/*
Lesson B1
File I/O in C

write(sock, &frame, sizeof(frame));   // send CAN frame
read(sock, &frame, sizeof(frame));    // receive CAN frame

        Standard C (stdio)          Linux System Calls
Functions   fopen, fread, fwrite, fclose    open, read, write, close
Returns     FILE * pointer       integer file descriptor
Buffered?   Yes — faster for text   No — direct kernel access
Use for     Text files, logs, config   Devices, sockets, precise control

FILE *logfile = fopen("can_log.txt", "w");
fprintf(logfile, "data here\n");
fclose(logfile);

FILE *f = fopen("filename.txt", "mode");

Mode                    Meaning
"r"                     Read only — file must exist
"w"                     Write — creates file, overwrites if exists
"a"                     Append — adds to end of existing file
"r+                     "Read and write
"b"                     Binary mode — add to any: "rb", "wb"

Always check if fopen succeeded:
FILE *f = fopen("data.txt", "r");
if (f == NULL) {
    perror("Failed to open file");
    return 1;
}

Reading Files — Three Ways
// 1. Read one character at a time
int c = fgetc(f);

// 2. Read one line at a time
char line[256];
fgets(line, sizeof(line), f);

// 3. Read formatted data (like scanf but from file)
int value;
fscanf(f, "%d", &value);

Writing Files — Three Ways
c// 1. Write one character
fputc('A', f);

// 2. Write a string
fputs("hello\n", f);

// 3. Write formatted (like printf but to file)
fprintf(f, "RPM: %d  Temp: %d\n", rpm, temp);

open — Open a File
#include <fcntl.h>

int fd = open("filename.txt", flags);
int fd = open("filename.txt", flags, mode);


Flag                Meaning
O_RDONLY            Read only
O_WRONLY            Write only
O_RDWR              Read and write
O_CREAT             Create if doesn't exist
O_TRUNC             Clear file on open
O_APPEND            Write to end


Combine flags with |:
int fd = open("log.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
The 0644 is the permission for the new file — same as chmod 644.

read and write
// Read
char buf[128];
int nbytes = read(fd, buf, sizeof(buf));

// Write
char *msg = "hello\n";
write(fd, msg, strlen(msg));

lseek — Move Position in a File
#include <unistd.h>

lseek(fd, 0, SEEK_SET);    // go to beginning
lseek(fd, 0, SEEK_END);    // go to end
lseek(fd, 10, SEEK_CUR);   // move 10 bytes forward from current position

Whence          Meaning
SEEK_SET        From beginning of file
SEEK_END        From end of file
SEEK_CUR        From current position

close
cclose(fd);

Practical Program — Sensor Data File

nano file_io.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define FILENAME "sensor_data.txt"

// ── Write sensor data using system calls ─────────────
void write_sensor_data() {
    int fd = open(FILENAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Failed to open file for writing");
        return;
    }

    printf("Writing sensor data...\n");

    // Simulate writing 5 sensor readings
    char buf[128];
    int readings[][2] = {
        {800,  90},
        {950,  91},
        {1100, 92},
        {1300, 93},
        {1500, 94},
    };

    for (int i = 0; i < 5; i++) {
        int len = snprintf(buf, sizeof(buf),
                  "RPM=%d TEMP=%d\n",
                  readings[i][0], readings[i][1]);
        write(fd, buf, len);
        printf("  Wrote: RPM=%d TEMP=%d\n",
               readings[i][0], readings[i][1]);
    }

    close(fd);
    printf("Write complete.\n\n");
}

// ── Read sensor data using stdio ─────────────────────
void read_sensor_data() {
    FILE *f = fopen(FILENAME, "r");
    if (f == NULL) {
        perror("Failed to open file for reading");
        return;
    }

    printf("Reading sensor data...\n");

    char line[128];
    int rpm, temp, count = 0;

    while (fgets(line, sizeof(line), f) != NULL) {
        // Parse each line
        if (sscanf(line, "RPM=%d TEMP=%d", &rpm, &temp) == 2) {
            count++;
            printf("  Reading %d: RPM=%d  TEMP=%d°C\n",
                   count, rpm, temp);
        }
    }

    fclose(f);
    printf("Read complete. Total readings: %d\n\n", count);
}

// ── Append new data using stdio ───────────────────────
void append_sensor_data(int rpm, int temp) {
    FILE *f = fopen(FILENAME, "a");
    if (f == NULL) {
        perror("Failed to open file for appending");
        return;
    }

    fprintf(f, "RPM=%d TEMP=%d\n", rpm, temp);
    fclose(f);
    printf("Appended: RPM=%d TEMP=%d\n", rpm, temp);
}

// ── Get file size using lseek ─────────────────────────
void show_file_size() {
    int fd = open(FILENAME, O_RDONLY);
    if (fd < 0) {
        perror("Failed to open file");
        return;
    }

    // lseek to end returns current position = file size
    long size = lseek(fd, 0, SEEK_END);
    close(fd);
    printf("File size: %ld bytes\n\n", size);
}

// ── Main ─────────────────────────────────────────────
int main() {
    printf("=== File I/O Demo ===\n\n");

    // 1. Write initial data
    write_sensor_data();

    // 2. Read it back
    read_sensor_data();

    // 3. Show file size
    show_file_size();

    // 4. Append more readings
    printf("Appending new readings...\n");
    append_sensor_data(1700, 95);
    append_sensor_data(1900, 96);
    printf("\n");

    // 5. Read again to see all data
    read_sensor_data();

    // 6. Final file size
    show_file_size();

    return 0;
}

Compile:
gcc file_io.c -o file_io
Run:
./file_io

=== File I/O Demo ===

Writing sensor data...
  Wrote: RPM=800  TEMP=90
  Wrote: RPM=950  TEMP=91
  Wrote: RPM=1100 TEMP=92
  Wrote: RPM=1300 TEMP=93
  Wrote: RPM=1500 TEMP=94
Write complete.

Reading sensor data...
  Reading 1: RPM=800  TEMP=90°C
  Reading 2: RPM=950  TEMP=91°C
  Reading 3: RPM=1100 TEMP=92°C
  Reading 4: RPM=1300 TEMP=93°C
  Reading 5: RPM=1500 TEMP=94°C
Read complete. Total readings: 5

File size: 85 bytes

Appending new readings...
Appended: RPM=1700 TEMP=95
Appended: RPM=1900 TEMP=96

Reading sensor data...
  Reading 1: RPM=800  TEMP=90°C
  Reading 2: RPM=950  TEMP=91°C
  Reading 3: RPM=1100 TEMP=92°C
  Reading 4: RPM=1300 TEMP=93°C
  Reading 5: RPM=1500 TEMP=94°C
  Reading 6: RPM=1700 TEMP=95°C
  Reading 7: RPM=1900 TEMP=96°C
Read complete. Total readings: 7

File size: 119 bytes


stdio (fopen/fclose)          system calls (open/close)
────────────────────          ─────────────────────────
FILE *f = fopen(...)          int fd = open(...)
fgets / fscanf                read(fd, buf, size)
fprintf / fputs               write(fd, buf, size)
fclose(f)                     lseek(fd, pos, whence)
                              close(fd)

Use for: text files,          Use for: devices, sockets,
logs, configs                 hardware, precise control
*/
