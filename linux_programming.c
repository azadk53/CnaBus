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

/*
Lesson B2
Error Handling in C

Why This Lesson Matters
In embedded systems things go wrong constantly:

A device file doesn't exist yet
A sensor stops responding
A file permission is wrong
A network interface goes down

When a system call fails it does two things:

Returns -1 (or NULL for pointer functions)
Sets a global variable called errno with the error code

int fd = open("/dev/nonexistent", O_RDONLY);
// fd = -1  ← failure
// errno = 2  ← ENOENT (No such file or directory)

errno value     Constant      Meaning
1               EPERM       Operation not permitted
2               ENOENT      No such file or directory
13              EACCES      Permission denied
16              EBUSY       Device busy
22              EINVAL      Invalid argument
32              EPIPE       Broken pipe
110             ETIMEDOUT   Connection timed out


perror() — Quickest Way
#include <stdio.h>

int fd = open("/dev/fake", O_RDONLY);
if (fd < 0) {
    perror("open failed");
}

open failed: No such file or directory

strerror() — Get Error as String
#include <string.h>
#include <errno.h>

int fd = open("/dev/fake", O_RDONLY);
if (fd < 0) {
    printf("Error code: %d\n", errno);
    printf("Error message: %s\n", strerror(errno));
}

Error code: 2
Error message: No such file or directory

errno directly — For specific error handling
#include <errno.h>

int fd = open("/dev/i2c-1", O_RDWR);
if (fd < 0) {
    if (errno == ENOENT) {
        printf("I2C device not found — is the module loaded?\n");
    } else if (errno == EACCES) {
        printf("Permission denied — try: sudo chmod 666 /dev/i2c-1\n");
    } else {
        perror("Unexpected error opening I2C device");
    }
}

errno gets overwritten by the next system call. Always read it right after a failure:
int fd = open("/dev/fake", O_RDONLY);
if (fd < 0) {
    int saved_errno = errno;       // save it immediately
    printf("Doing something else...\n");  // this might change errno
    printf("Error was: %s\n", strerror(saved_errno));  // use saved
}


void die(const char *msg) {
    perror(msg);
    exit(1);
}

// Usage — much cleaner
int fd = open("/dev/spidev0.0", O_RDWR);
if (fd < 0) die("Failed to open SPI device");

int n = read(fd, buf, size);
if (n < 0) die("Failed to read from SPI device");

nano error_handling.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

// ── Helper: print error and exit ─────────────────────
void die(const char *msg) {
    fprintf(stderr, "FATAL: %s: %s\n", msg, strerror(errno));
    exit(1);
}

// ── Helper: print warning but continue ───────────────
void warn(const char *msg) {
    fprintf(stderr, "WARNING: %s: %s\n", msg, strerror(errno));
}

// ── Try to open a file safely ────────────────────────
int safe_open(const char *path, int flags) {
    int fd = open(path, flags);
    if (fd < 0) {
        int saved = errno;
        fprintf(stderr, "ERROR opening '%s': ", path);

        switch(saved) {
            case ENOENT:
                fprintf(stderr, "File not found\n");
                break;
            case EACCES:
                fprintf(stderr, "Permission denied — check chmod\n");
                break;
            case EBUSY:
                fprintf(stderr, "Device busy — already in use\n");
                break;
            default:
                fprintf(stderr, "%s\n", strerror(saved));
        }
        return -1;
    }
    return fd;
}

// ── Write with error checking ─────────────────────────
int safe_write(int fd, const char *data, int len) {
    int written = write(fd, data, len);
    if (written < 0) {
        warn("write failed");
        return -1;
    }
    if (written < len) {
        fprintf(stderr, "WARNING: partial write (%d of %d bytes)\n",
                written, len);
        return -1;
    }
    return written;
}

// ── Read with error checking ──────────────────────────
int safe_read(int fd, char *buf, int len) {
    int nbytes = read(fd, buf, len);
    if (nbytes < 0) {
        warn("read failed");
        return -1;
    }
    if (nbytes == 0) {
        printf("End of file reached\n");
        return 0;
    }
    return nbytes;
}

// ── Main ─────────────────────────────────────────────
int main() {
    printf("=== Error Handling Demo ===\n\n");

    // 1. Try opening a file that doesn't exist
    printf("Test 1: Open non-existent file\n");
    int fd = safe_open("/tmp/does_not_exist.txt", O_RDONLY);
    if (fd < 0) printf("  Handled gracefully.\n\n");

    // 2. Try opening a real file
    printf("Test 2: Open real file\n");
    fd = safe_open("/proc/version", O_RDONLY);
    if (fd >= 0) {
        char buf[256];
        int n = safe_read(fd, buf, sizeof(buf)-1);
        if (n > 0) {
            buf[n] = '\0';
            printf("  Read: %s\n", buf);
        }
        close(fd);
    }

    // 3. Create and write to a file
    printf("Test 3: Write to file\n");
    fd = open("/tmp/test_output.txt",
              O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) die("Cannot create test file");

    char *data = "sensor=RPM value=1500\n";
    int result = safe_write(fd, data, strlen(data));
    if (result > 0) {
        printf("  Wrote %d bytes successfully\n", result);
    }
    close(fd);

    // 4. Try to write to a read-only file descriptor
    printf("\nTest 4: Write to read-only fd\n");
    fd = open("/tmp/test_output.txt", O_RDONLY);
    if (fd >= 0) {
        result = safe_write(fd, "test", 4);
        if (result < 0) printf("  Handled gracefully.\n");
        close(fd);
    }

    // 5. Show errno values directly
    printf("\nTest 5: errno values\n");
    open("/nonexistent", O_RDONLY);
    printf("  errno after failed open: %d (%s)\n",
           errno, strerror(errno));

    printf("\nDone.\n");
    return 0;
}

gcc error_handling.c -o error_handling
./error_handling
=== Error Handling Demo ===

Test 1: Open non-existent file
  ERROR opening '/tmp/does_not_exist.txt': File not found
  Handled gracefully.

Test 2: Open real file
  Read: Linux version 6.1.0-microsoft-standard-WSL2 ...

Test 3: Write to file
  Wrote 22 bytes successfully

Test 4: Write to read-only fd
  WARNING: write failed: Bad file descriptor
  Handled gracefully.

Test 5: errno values
  errno after failed open: 2 (No such file or directory)

Done.

stderr vs stdout
You might have noticed fprintf(stderr, ...) in the code. There are actually three standard streams in Linux:
Stream         fd      Purpose     Command
stdin          0       Input       keyboard
stdout         1       Normal output    printf()
stderr         2       Error output  fprintf(stderr,...)


// Standard embedded error checking pattern:

int fd = open(device_path, O_RDWR);
if (fd < 0) {
    perror("open");
    return -1;         // or exit(1) if fatal
}

int n = read(fd, buf, size);
if (n < 0) {
    perror("read");
    close(fd);
    return -1;
}

// success — use the data


Tool                 Use case
perror("msg")       Quick error print — message + errno description
strerror(errno)     Get error as string — use in fprintf or log files
errno               Check specific error type with if/switch
fprintf(stderr,...) Print errors to stderr not stdout
Save errno immediately  Before any other call overwrites it
die() helper            Fatal errors — print and exit
warn() helper           Non-fatal — log and continue
*/
/*
Lesson B3
Processes in C

What is a Process in Code?
In Lesson A4 you managed processes from the terminal — ps, kill, &. Now we go one level deeper — creating and managing processes from C code.

Main embedded program
├── spawns a logging process
├── spawns a sensor reading process
└── watches both — restarts if one crashes

Function        What it does
fork()          Creates a copy of the current process
exec()          Replaces current process with a new program
wait()          Parent waits for child to finish

pid_t pid = fork();
Parent process          Child process
──────────────          ─────────────
pid = child's PID       pid = 0
pid_t pid = fork();

if (pid < 0) {
    perror("fork failed");
    exit(1);
} else if (pid == 0) {
    // This code runs in the CHILD
    printf("I am the child\n");
} else {
    // This code runs in the PARENT
    printf("I am the parent, child PID = %d\n", pid);
}

#include <sys/wait.h>

int status;
wait(&status);    // blocks until any child finishes

// Check how child exited
if (WIFEXITED(status)) {
    printf("Child exited with code: %d\n", WEXITSTATUS(status));
}

Macro                    Meaning
WIFEXITED(status)       Did child exit normally?WEXITSTATUS(status)     What exit code did it return?WIFSIGNALED(status)    Was child killed by a signal?WTERMSIG(status)       Which signal killed it?

#include <unistd.h>

char *args[] = {"candump", "vcan0", NULL};  // NULL terminates
execvp("candump", args);

// If execvp returns, it failed
perror("execvp failed");

pid_t pid = fork();

if (pid == 0) {
    // Child — replace with new program
    char *args[] = {"candump", "vcan0", NULL};
    execvp("candump", args);
    perror("exec failed");
    exit(1);
} else {
    // Parent — wait for child
    int status;
    wait(&status);
    printf("candump finished\n");
}

nano processes.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>

// ── Worker: simulate a sensor reading task ────────────
void sensor_worker(int sensor_id, int readings) {
    printf("[Worker %d] PID=%d started\n",
           sensor_id, getpid());

    for (int i = 1; i <= readings; i++) {
        // Simulate reading a sensor value
        int value = 100 + (sensor_id * 10) + i;
        printf("[Worker %d] Reading %d: value=%d\n",
               sensor_id, i, value);
        sleep(1);
    }

    printf("[Worker %d] Done. Exiting.\n", sensor_id);
    exit(0);
}

// ── Spawn a worker process ────────────────────────────
pid_t spawn_worker(int sensor_id, int readings) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return -1;
    }

    if (pid == 0) {
        // Child process — run the worker
        sensor_worker(sensor_id, readings);
        // sensor_worker calls exit() so we never reach here
    }

    // Parent — return child PID
    printf("[Parent] Spawned worker %d with PID=%d\n",
           sensor_id, pid);
    return pid;
}

// ── Wait for all children ─────────────────────────────
void wait_for_all(pid_t pids[], int count) {
    for (int i = 0; i < count; i++) {
        int status;
        pid_t done = waitpid(pids[i], &status, 0);

        if (done < 0) {
            perror("waitpid failed");
            continue;
        }

        if (WIFEXITED(status)) {
            printf("[Parent] Worker PID=%d finished "
                   "with exit code %d\n",
                   done, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("[Parent] Worker PID=%d killed "
                   "by signal %d\n",
                   done, WTERMSIG(status));
        }
    }
}

// ── Run an external command ───────────────────────────
void run_command(char *cmd, char *args[]) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return;
    }

    if (pid == 0) {
        // Child — exec the command
        execvp(cmd, args);
        // Only reaches here if exec failed
        fprintf(stderr, "Failed to run '%s': %s\n",
                cmd, strerror(errno));
        exit(1);
    }

    // Parent — wait for command to finish
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
        printf("Command '%s' exited with code %d\n",
               cmd, WEXITSTATUS(status));
    }
}

// ── Main ─────────────────────────────────────────────
int main() {
    printf("=== Process Manager Demo ===\n");
    printf("[Parent] PID=%d\n\n", getpid());

    // ── Test 1: Spawn multiple sensor workers ─────────
    printf("--- Test 1: Parallel sensor workers ---\n");

    pid_t workers[3];
    workers[0] = spawn_worker(1, 2);  // sensor 1, 2 readings
    workers[1] = spawn_worker(2, 2);  // sensor 2, 2 readings
    workers[2] = spawn_worker(3, 2);  // sensor 3, 2 readings

    printf("[Parent] All workers spawned. Waiting...\n\n");

    wait_for_all(workers, 3);
    printf("\nAll workers done.\n\n");

    // ── Test 2: Run external command ──────────────────
    printf("--- Test 2: Run external command ---\n");
    char *args[] = {"echo", "Hello from child process!", NULL};
    run_command("echo", args);

    // ── Test 3: Run ls command ────────────────────────
    printf("\n--- Test 3: List files via child ---\n");
    char *ls_args[] = {"ls", "-l", "/tmp", NULL};
    run_command("ls", ls_args);

    printf("\n=== Done ===\n");
    return 0;
}

gcc processes.c -o processes
./processes

output
=== Process Manager Demo ===
[Parent] PID=1234

--- Test 1: Parallel sensor workers ---
[Parent] Spawned worker 1 with PID=1235
[Parent] Spawned worker 2 with PID=1236
[Parent] Spawned worker 3 with PID=1237
[Parent] All workers spawned. Waiting...

[Worker 1] PID=1235 started
[Worker 1] Reading 1: value=111
[Worker 2] PID=1236 started
[Worker 2] Reading 1: value=121
[Worker 3] PID=1237 started
[Worker 3] Reading 1: value=131
[Worker 1] Reading 2: value=112
[Worker 2] Reading 2: value=122
[Worker 3] Reading 2: value=132
[Worker 1] Done. Exiting.
[Worker 2] Done. Exiting.
[Worker 3] Done. Exiting.
[Parent] Worker PID=1235 finished with exit code 0
[Parent] Worker PID=1236 finished with exit code 0
[Parent] Worker PID=1237 finished with exit code 0

All workers done.

--- Test 2: Run external command ---
Hello from child process!
Command 'echo' exited with code 0

--- Test 3: List files via child ---
total 12
-rw-r--r-- 1 azad azad 22 Jun 10 test_output.txt
Command 'ls' exited with code 0

=== Done ===

main process (PID=1234)
│
├── fork() ──► child 1 (PID=1235) → sensor_worker(1)
│                                    reads 2 values
│                                    exit(0)
│
├── fork() ──► child 2 (PID=1236) → sensor_worker(2)
│                                    reads 2 values
│                                    exit(0)
│
├── fork() ──► child 3 (PID=1237) → sensor_worker(3)
│                                    reads 2 values
│                                    exit(0)
│
└── wait_for_all() ─── blocks until all 3 finish

Function            Purpose
fork()              Create child process — copy of current
getpid()            Get current process PID
getppid()           Get parent process PID
wait(&status)       Wait for any child
waitpid(pid, &status, 0)        Wait for specific child
execvp(cmd, args[])     Replace process with new program
exit(code)          Exit current process with code
WIFEXITED(status)       Check if child exited normally
WEXITSTATUS(status)     Get child's exit code

main supervisor process
├── fork → exec → CAN logger      (records all bus traffic)
├── fork → exec → sensor reader   (reads I2C/SPI sensors)
├── fork → exec → data uploader   (sends data to server)
└── watchdog loop:
      wait for any child to exit
      if child crashed → restart it


Before fork():     one process
                        │
                      fork()
                     /      \
After fork():   Parent      Child
                (pid>0)     (pid==0)
                  │              │
               waits          does work
                  │              │
               wait()        exit(0)
                  │
             continues



*/

/*
Lesson B4
Signals in C

Signals are the Linux way of sending a notification to a process — like a tap on the shoulder telling it something happened.

Signal  Number     Default action       Sent by
SIGINT  2           Terminate           Ctrl+C
SIGTERM 15          Terminate           kill PID
SIGKILL 9           Force kill          kill -9 PID
SIGSEGV 11          Crash              Bad memory access
SIGALRM 14          Terminate           Timer expiry
SIGCHLD 17          Ignore           Child process exits
SIGHUP  1           Terminate       Terminal closed
SIGUSR1 10          Terminate          User defined
SIGUSR2 12          Terminate           User defined

// 1. Handle it — run your own function
signal(SIGINT, my_handler);

// 2. Ignore it — do nothing
signal(SIGINT, SIG_IGN);

// 3. Restore default behavior
signal(SIGINT, SIG_DFL);

void handle_exit(int sig) {
    running = 0;
}
signal(SIGINT, handle_exit);

The handler function must always have this signature:
void handler_name(int sig)


#include <signal.h>

struct sigaction sa;
sa.sa_handler = my_handler;    // your handler function
sigemptyset(&sa.sa_mask);      // don't block other signals
sa.sa_flags = 0;               // no special flags

sigaction(SIGINT, &sa, NULL);

signal(SIGALRM, timeout_handler);
alarm(5);    // send SIGALRM after 5 seconds

Process A sends SIGUSR1 to Process B
→ Process B interprets it as "reload config"
→ Process B interprets SIGUSR2 as "dump status"

nano signals.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>

// ── Global state ──────────────────────────────────────
volatile int running    = 1;
volatile int usr1_count = 0;
volatile int alarm_fired = 0;
int frame_count = 0;

// ── Signal handlers ───────────────────────────────────

// Ctrl+C — clean shutdown
void handle_sigint(int sig) {
    printf("\n[SIGINT] Ctrl+C received — shutting down...\n");
    running = 0;
}

// SIGTERM — from kill command
void handle_sigterm(int sig) {
    printf("\n[SIGTERM] Terminate signal received\n");
    running = 0;
}

// SIGUSR1 — custom: print status
void handle_sigusr1(int sig) {
    usr1_count++;
    // Note: printf is not safe in signal handlers
    // in real code use a flag and print in main loop
    // here we use it for demonstration only
    printf("\n[SIGUSR1] Status request #%d received\n",
           usr1_count);
    printf("  frames processed: %d\n", frame_count);
    printf("  still running: %s\n", running ? "yes" : "no");
}

// SIGALRM — timer expired
void handle_sigalrm(int sig) {
    alarm_fired = 1;
}

// SIGSEGV — catch crash (demonstration only)
void handle_sigsegv(int sig) {
    fprintf(stderr, "[SIGSEGV] Segmentation fault caught!\n");
    fprintf(stderr, "Bad memory access — check your pointers\n");
    exit(1);
}

// ── Setup all signal handlers ─────────────────────────
void setup_signals() {
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sa.sa_handler = handle_sigint;
    sigaction(SIGINT, &sa, NULL);

    sa.sa_handler = handle_sigterm;
    sigaction(SIGTERM, &sa, NULL);

    sa.sa_handler = handle_sigusr1;
    sigaction(SIGUSR1, &sa, NULL);

    sa.sa_handler = handle_sigalrm;
    sigaction(SIGALRM, &sa, NULL);

    sa.sa_handler = handle_sigsegv;
    sigaction(SIGSEGV, &sa, NULL);

    printf("Signal handlers registered.\n");
}

// ── Main ─────────────────────────────────────────────
int main() {
    printf("=== Signal Demo ===\n");
    printf("My PID = %d\n", getpid());
    printf("Try these from another terminal:\n");
    printf("  kill -USR1 %d   (print status)\n", getpid());
    printf("  kill -TERM %d   (terminate)\n", getpid());
    printf("  Ctrl+C          (interrupt)\n\n");

    setup_signals();

    // Set a 10 second alarm
    alarm(10);
    printf("10 second alarm set.\n\n");

    // Main processing loop
    printf("Running... (press Ctrl+C to stop)\n");
    while (running) {
        // Simulate processing work
        frame_count++;

        // Check if alarm fired
        if (alarm_fired) {
            alarm_fired = 0;
            printf("[ALARM] 10 seconds elapsed! "
                   "Frames processed: %d\n", frame_count);
            alarm(10);    // reset alarm for next 10 seconds
        }

        sleep(1);
    }

    // Cleanup
    printf("\nShutting down cleanly.\n");
    printf("Total frames processed: %d\n", frame_count);
    return 0;
}


gcc signals.c -o signals
./signals

output
=== Signal Demo ===
My PID = 1234
Try these from another terminal:
  kill -USR1 1234   (print status)
  kill -TERM 1234   (terminate)
  Ctrl+C            (interrupt)

Signal handlers registered.
10 second alarm set.

Running... (press Ctrl+C to stop)

Window 2 — test each signal:

# Send SIGUSR1 — print status
kill -USR1 1234

# Wait a few seconds, send again
kill -USR1 1234

# Finally terminate it
kill -TERM 1234

ouput window 1
Running... (press Ctrl+C to stop)

[SIGUSR1] Status request #1 received
  frames processed: 3
  still running: yes

[SIGUSR1] Status request #2 received
  frames processed: 7
  still running: yes

[ALARM] 10 seconds elapsed! Frames processed: 10

[SIGTERM] Terminate signal received

Shutting down cleanly.
Total frames processed: 11

#include <signal.h>

// Send signal to a specific PID
kill(target_pid, SIGUSR1);

// Send signal to yourself
kill(getpid(), SIGUSR1);

// Send signal to all processes in your group
kill(0, SIGTERM);

// Simple assignments
running = 0;
flag = 1;

// Write to file descriptor directly
write(fd, "msg\n", 4);

printf(...)      // not signal safe
malloc(...)      // not signal safe
fopen(...)       // not signal safe

// Handler — just sets a flag
void handle_sigusr1(int sig) {
    print_status = 1;    // just set flag
}

// Main loop — checks flag and acts
while (running) {
    if (print_status) {
        print_status = 0;
        printf("Status: frames=%d\n", frame_count);  // safe here
    }
    // do other work
}

Signal      Embedded use
SIGINT / SIGTERM    Clean shutdown — flush buffers, close devices
SIGALRM     Watchdog timer — restart if no heartbeat
SIGUSR1     Reload config without restarting
SIGUSR2     Dump debug info to log file
SIGCHLD     Detect when a worker process crashes
SIGSEGV     Catch crash, log state before dying



*/

/*
Lesson B5
Threads with pthreads

Process                          Thread
───────                          ──────
separate memory space            shares memory with other threads
created with fork()              created with pthread_create()
heavy — slow to create           light — fast to create
independent — crash stays local  crash can affect whole program
CAN logger vs sensor reader      RPM reader + Temp reader together

Your CAN logger program
├── Thread 1: receive CAN frames (blocking read)
├── Thread 2: write frames to file
└── Thread 3: print live stats every second

#include <pthread.h>

gcc program.c -o program -lpthread

Function            Purpose
pthread_create()    Start a new thread
pthread_join()      Wait for thread to finish
pthread_exit()      Exit current thread
pthread_self()      Get current thread ID
pthread_mutex_lock()    Lock a mutex
pthread_mutex_unlock()  Unlock a mutex

pthread_t thread;    // thread identifier

pthread_create(&thread, NULL, my_function, arg);

&thread      →  stores thread ID here
NULL         →  default thread attributes
my_function  →  function to run in new thread
arg          →  argument to pass to the function (or NULL)

void *my_function(void *arg) {
    // thread code here
    return NULL;
}

pthread_join(thread, NULL);

Thread 1: reads frame_count (value = 5)
Thread 2: reads frame_count (value = 5)
Thread 1: adds 1, writes 6
Thread 2: adds 1, writes 6   ← should be 7!

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Thread 1
pthread_mutex_lock(&mutex);    // lock — only one thread enters
frame_count++;                 // safe now
pthread_mutex_unlock(&mutex);  // unlock — other threads can enter

// Thread 2
pthread_mutex_lock(&mutex);    // waits if Thread 1 holds lock
frame_count++;
pthread_mutex_unlock(&mutex);


nano threads.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>

#define MAX_FRAMES 1000
#define FRAME_SIZE 64

// ── Shared data structure ─────────────────────────────
typedef struct {
    int  id;
    char data[FRAME_SIZE];
    long timestamp;
} Frame;

// ── Shared buffer ─────────────────────────────────────
Frame    frame_buffer[MAX_FRAMES];
int      frame_count  = 0;
int      frames_written = 0;
volatile int running  = 1;

// ── Mutex to protect shared data ─────────────────────
pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;

// ── Signal handler ────────────────────────────────────
void handle_exit(int sig) {
    running = 0;
}

// ── Get timestamp in milliseconds ────────────────────
long get_ms() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

// ── Thread 1: Simulate receiving CAN frames ───────────
void *receiver_thread(void *arg) {
    printf("[Receiver] Thread started\n");

    int can_ids[] = {0x101, 0x200, 0x300, 0x102};
    int id_count  = 4;
    int seq       = 0;

    while (running) {
        // Simulate receiving a CAN frame every 200ms
        usleep(200000);

        Frame f;
        f.id        = can_ids[seq % id_count];
        f.timestamp = get_ms();
        snprintf(f.data, FRAME_SIZE,
                 "DE AD BE EF %02X", seq & 0xFF);

        // Lock buffer before writing
        pthread_mutex_lock(&buffer_mutex);

        if (frame_count < MAX_FRAMES) {
            frame_buffer[frame_count] = f;
            frame_count++;
        }

        pthread_mutex_unlock(&buffer_mutex);

        seq++;
    }

    printf("[Receiver] Thread stopped\n");
    return NULL;
}

// ── Thread 2: Write frames to file ───────────────────
void *writer_thread(void *arg) {
    printf("[Writer] Thread started\n");

    FILE *f = fopen("thread_log.txt", "w");
    if (!f) {
        perror("Failed to open log file");
        return NULL;
    }

    fprintf(f, "timestamp    ID       Data\n");
    fprintf(f, "─────────────────────────────────\n");

    while (running || frames_written < frame_count) {
        pthread_mutex_lock(&buffer_mutex);

        // Write any unwritten frames
        while (frames_written < frame_count) {
            Frame *fr = &frame_buffer[frames_written];
            fprintf(f, "%-12ld 0x%03X    %s\n",
                    fr->timestamp, fr->id, fr->data);
            frames_written++;
        }

        pthread_mutex_unlock(&buffer_mutex);

        usleep(100000);    // check every 100ms
    }

    fclose(f);
    printf("[Writer] Thread stopped. "
           "Wrote %d frames.\n", frames_written);
    return NULL;
}

// ── Thread 3: Print live statistics ──────────────────
void *stats_thread(void *arg) {
    printf("[Stats] Thread started\n");

    int last_count = 0;

    while (running) {
        sleep(2);    // print stats every 2 seconds

        pthread_mutex_lock(&buffer_mutex);
        int current = frame_count;
        pthread_mutex_unlock(&buffer_mutex);

        int rate = (current - last_count) / 2;
        printf("[Stats] Total frames: %d  "
               "Rate: %d frames/sec\n",
               current, rate);
        last_count = current;
    }

    printf("[Stats] Thread stopped\n");
    return NULL;
}

// ── Main ─────────────────────────────────────────────
int main() {
    printf("=== Multithreaded CAN Logger ===\n");
    printf("Press Ctrl+C to stop\n\n");

    // Setup signal handler
    struct sigaction sa;
    sa.sa_handler = handle_exit;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    // Create threads
    pthread_t receiver, writer, stats;

    pthread_create(&receiver, NULL, receiver_thread, NULL);
    pthread_create(&writer,   NULL, writer_thread,   NULL);
    pthread_create(&stats,    NULL, stats_thread,    NULL);

    printf("All threads started.\n\n");

    // Wait for all threads to finish
    pthread_join(receiver, NULL);
    pthread_join(writer,   NULL);
    pthread_join(stats,    NULL);

    // Final summary
    printf("\n=== Summary ===\n");
    printf("Total frames captured: %d\n", frame_count);
    printf("Total frames written:  %d\n", frames_written);
    printf("Log saved to: thread_log.txt\n");

    return 0;
}

gcc threads.c -o threads -lpthread
./threads
=== Multithreaded CAN Logger ===
Press Ctrl+C to stop

[Receiver] Thread started
[Writer]   Thread started
[Stats]    Thread started
All threads started.

[Stats] Total frames: 10  Rate: 5 frames/sec
[Stats] Total frames: 20  Rate: 5 frames/sec
[Stats] Total frames: 30  Rate: 5 frames/sec
^C
[Receiver] Thread stopped
[Stats]    Thread stopped
[Writer]   Thread stopped. Wrote 32 frames.

=== Summary ===
Total frames captured: 32
Total frames written:  32
Log saved to: thread_log.txt

cat thread_log.txt

Main thread
│
├── pthread_create → Receiver thread
│                    every 200ms:
│                    lock mutex
│                    add frame to buffer
│                    unlock mutex
│
├── pthread_create → Writer thread
│                    every 100ms:
│                    lock mutex
│                    write unwritten frames to file
│                    unlock mutex
│
└── pthread_create → Stats thread
                     every 2 seconds:
                     lock mutex
                     read frame_count
                     unlock mutex
                     print stats

Time ──────────────────────────────────────►

Receiver:  [lock]─write─[unlock]
Writer:                         [lock]─write─[unlock]
Stats:             waiting...           [lock]─read─[unlock]

// Define your argument struct
typedef struct {
    int sensor_id;
    int interval_ms;
} ThreadArgs;

// Thread function receives it as void*
void *sensor_thread(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;  // cast back
    printf("Sensor %d, interval %dms\n",
           args->sensor_id, args->interval_ms);
    return NULL;
}

// Pass it when creating thread
ThreadArgs args = {1, 100};
pthread_create(&thread, NULL, sensor_thread, &args);


Concept             Code
Create thread       pthread_create(&t, NULL, func, arg)
Wait for thread     pthread_join(t, NULL)
Exit thread         return NULL or pthread_exit(NULL)
Get thread ID       pthread_self()
Initialize mutex    pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER
Lock mutex          pthread_mutex_lock(&m)
Unlock mutex        pthread_mutex_unlock(&m)
Compile flag        -lpthread


*/