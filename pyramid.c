#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>

#define FOV 30  // Increase the field of view slightly
#define ZOOM 100  // Adjusted zoom for better projection

// Larger coordinates for the pyramid
float vertices[5][3] = {
    {0, 2, 0},     // Top point
    {-2, -2, 2},   // Base 1
    {2, -2, 2},    // Base 2
    {2, -2, -2},   // Base 3
    {-2, -2, -2}   // Base 4
};

int edges[8][2] = {
    {0, 1}, {0, 2}, {0, 3}, {0, 4}, // Top to base points
    {1, 2}, {2, 3}, {3, 4}, {4, 1}  // Base edges
};

// Function to get the terminal width and height
void get_terminal_size(int *width, int *height) {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);  // Get the terminal size
    *width = w.ws_col;
    *height = w.ws_row;
}

// Function to rotate a point
void rotate(float x, float y, float z, float* new_x, float* new_y, float* new_z, float angleX, float angleY, float angleZ) {
    float cosa = cos(angleX), sina = sin(angleX);
    float cosb = cos(angleY), sinb = sin(angleY);
    float cosc = cos(angleZ), sinc = sin(angleZ);
    
    float newx = x, newy = y, newz = z;
    
    float temp_y = cosa * newy - sina * newz;
    newz = sina * newy + cosa * newz;
    newy = temp_y;

    float temp_x = cosb * newx + sinb * newz;
    newz = -sinb * newx + cosb * newz;
    newx = temp_x;

    temp_x = cosc * newx - sinc * newy;
    newy = sinc * newx + cosc * newy;
    newx = temp_x;

    *new_x = newx;
    *new_y = newy;
    *new_z = newz;
}

// Project 3D points onto 2D screen
void project(float x, float y, float z, int* screen_x, int* screen_y, int screen_width, int screen_height) {
    *screen_x = (int)(screen_width / 2 + ZOOM * x / (z + FOV));
    *screen_y = (int)(screen_height / 2 - ZOOM * y / (z + FOV));
}

// Main rendering function
void render_frame(float angleX, float angleY, float angleZ, int screen_width, int screen_height) {
    char screen[screen_height][screen_width];
    for (int i = 0; i < screen_height; ++i)
        for (int j = 0; j < screen_width; ++j)
            screen[i][j] = ' ';

    for (int i = 0; i < 8; ++i) {
        float x1 = vertices[edges[i][0]][0];
        float y1 = vertices[edges[i][0]][1];
        float z1 = vertices[edges[i][0]][2];
        float x2 = vertices[edges[i][1]][0];
        float y2 = vertices[edges[i][1]][1];
        float z2 = vertices[edges[i][1]][2];

        float new_x1, new_y1, new_z1;
        float new_x2, new_y2, new_z2;

        rotate(x1, y1, z1, &new_x1, &new_y1, &new_z1, angleX, angleY, angleZ);
        rotate(x2, y2, z2, &new_x2, &new_y2, &new_z2, angleX, angleY, angleZ);

        int screen_x1, screen_y1, screen_x2, screen_y2;

        project(new_x1, new_y1, new_z1, &screen_x1, &screen_y1, screen_width, screen_height);
        project(new_x2, new_y2, new_z2, &screen_x2, &screen_y2, screen_width, screen_height);

        int dx = abs(screen_x2 - screen_x1), sx = screen_x1 < screen_x2 ? 1 : -1;
        int dy = -abs(screen_y2 - screen_y1), sy = screen_y1 < screen_y2 ? 1 : -1; 
        int err = dx + dy, e2;

        while (1) {
            if (screen_x1 >= 0 && screen_x1 < screen_width && screen_y1 >= 0 && screen_y1 < screen_height)
                screen[screen_y1][screen_x1] = '#';
            if (screen_x1 == screen_x2 && screen_y1 == screen_y2) break;
            e2 = 2 * err;
            if (e2 >= dy) { err += dy; screen_x1 += sx; }
            if (e2 <= dx) { err += dx; screen_y1 += sy; }
        }
    }

    for (int i = 0; i < screen_height; ++i) {
        for (int j = 0; j < screen_width; ++j)
            putchar(screen[i][j]);
        putchar('\n');
    }
}

int main() {
    float angleX = 0, angleY = 0, angleZ = 0;
    int screen_width, screen_height;

    while (1) {
        get_terminal_size(&screen_width, &screen_height);  // Get the current terminal size
        render_frame(angleX, angleY, angleZ, screen_width, screen_height);
        angleX += 0.04;
        angleY += 0.05;
        angleZ += 0.03;
        usleep(50000);  // Slow down the rotation for better visualization
        printf("\033[H\033[J");  // Clear the screen for animation
    }

    return 0;
}
