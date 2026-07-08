// Linux File System //

// File System Tree //

/*
/                        ← root, top of everything
├── bin/                 ← essential programs (ls, cp, cat)
├── boot/                ← kernel and bootloader files
├── dev/                 ← device files (hardware as files)
│   ├── ttyS0            ← serial port
│   ├── spidev0.0        ← SPI device
│   └── vcan0            ← your CAN interface
├── etc/                 ← system configuration files
│   └── systemd/         ← service configurations
├── home/                ← user home directories
│   └── azad/            ← your personal files
├── lib/                 ← shared libraries
├── mnt/                 ← mount points for drives
├── proc/                ← running processes as files
│   ├── cpuinfo          ← CPU information
│   └── meminfo          ← memory information
├── sys/                 ← hardware info from kernel
│   └── class/
│       └── gpio/        ← GPIO pins as files
├── tmp/                 ← temporary files
├── usr/                 ← installed software
│   ├── bin/             ← user programs
│   └── include/         ← header files (like linux/can.h)
└── var/                 ← logs and variable data
    └── log/             ← system logs
*/

/*

/dev/Hardware devices — SPI, I2C, UART, GPIO
/sys/Read hardware info, control GPIO pins
/proc/Read CPU, memory, process information
/etc/systemd/Register your program to run on boot
/usr/include/Header files for C programming
/var/log/System logs for debugging

*/
/*
(whats here)
pwd- where am i
ls -l        # detailed list with permissions and sizes
ls -la       # same but also shows hidden files (starting with .)
ls -lh       # human readable file sizes (KB, MB instead of bytes)
cd /          # go to root
ls            # see everything at root level
(move around)
cd /dev       # go to devices folder
ls            # see all device files

cd ~          # go back to your home directory (~ means home)
pwd           # confirm you're home
(create folder)
cd ~                    # make sure you're home
mkdir can_projects      # create a folder for our work
ls                      # verify it was created
cd can_projects         # go into it
pwd                     # confirm location
touch notes.txt         # creates empty file
ls -l                   # see it in the list
cp notes.txt notes_backup.txt    # copy
ls                               # see both files
mv notes_backup.txt old_notes.txt   # rename
ls                                  # see new name
rm old_notes.txt      # delete file
ls                    # confirm it's gone
cat /proc/cpuinfo     # read CPU information
cat /proc/meminfo     # memory information
clear
(practice code)

# 1. See where you are
pwd

# 2. Go to root and look around
cd /
ls

# 3. Go into /proc and read CPU info
cd /proc
cat cpuinfo

# 4. Go home and create a project folder
cd ~
mkdir embedded_linux
cd embedded_linux
pwd

# 5. Create some files
touch lesson1.txt
touch lesson2.txt
ls -l

# 6. Copy and rename
cp lesson1.txt lesson1_backup.txt
mv lesson2.txt lesson2_notes.txt
ls

# 7. Clean up one file
rm lesson1_backup.txt
ls

*/

/*
Phase A — Lesson 2
File Permissions

Who can READ it?
Who can WRITE it?
Who can EXECUTE it?
Owner   → the user who created the file
Group   → a group of users
Others  → everyone else
ls -l ~
-rwxr-xr-- 1 azad azad 1234 Jun 10 12:00 can_send
drwxr-xr-x 2 azad azad 4096 Jun 10 11:00 embedded_linux
-rw-r--r-- 1 azad azad  128 Jun 10 11:30 notes.txt
-  rwx  r-x  r--
│   │    │    │
│   │    │    └── Others permissions
│   │    └─────── Group permissions
│   └──────────── Owner permissions
└──────────────── File type (- = file, d = directory)
rwxr-xr--
│││ │││ │││
│││ │││ └└└── Others: r-- = read only
│││ └└└────── Group:  r-x = read and execute
└└└────────── Owner:  rwx = read, write, execute
rw-r--r--   ← typical text file
rwxr-xr-x   ← typical program/script
rwx------   ← private, only owner can access

chmod u+x script.sh    # give owner execute permission
chmod g+w file.txt     # give group write permission
chmod o-r secret.txt   # remove read from others
chmod a+r file.txt     # give everyone read (a = all)
r = 4
w = 2
x = 1
- = 0
rwx = 4+2+1 = 7
r-x = 4+0+1 = 5
r-- = 4+0+0 = 4
--- = 0+0+0 = 0
7 = rwx  → owner can read, write, execute
5 = r-x  → group can read and execute
5 = r-x  → others can read and execute
# 1. Go home and create a test file
cd ~
touch test_permissions.txt
ls -l test_permissions.txt

# 2. Check default permissions
ls -l test_permissions.txt
# should show -rw-r--r-- (644)

# 3. Remove all permissions
chmod 000 test_permissions.txt
ls -l test_permissions.txt
# should show ----------

# 4. Try to read it
cat test_permissions.txt
# should get: Permission denied

# 5. Give yourself read permission back
chmod 600 test_permissions.txt
cat test_permissions.txt
# works now (file is empty but no error)

# 6. Create a script and make it executable
touch my_script.sh
echo '#!/bin/bash' > my_script.sh
echo 'echo Hello from my script' >> my_script.sh
ls -l my_script.sh
# shows -rw-r--r-- (not executable yet)

# 7. Try to run it without execute permission
./my_script.sh
# Permission denied

# 8. Add execute permission
chmod 755 my_script.sh
ls -l my_script.sh
# now shows -rwxr-xr-x

# 9. Run it
./my_script.sh
# Hello from my script
chown azad file.txt           # change owner to azad
chown azad:students file.txt  # change owner and group
crw-rw---- 1 root i2c 89, 1 Jun 10 /dev/i2c-1
# Option 1 — add yourself to the i2c group
sudo usermod -aG i2c azad

# Option 2 — change permissions temporarily
sudo chmod 666 /dev/i2c-1
Permission string:  - r w x r - x r - -
                    │ │ │ │ │ │ │ │ │ │
                    │ └─┴─┘ └─┴─┘ └─┴─┘
                    │ owner  group others
                    │
                    └── file type (- or d)

chmod 755 = rwxr-xr-x = owner all, group/others read+execute
chmod 644 = rw-r--r-- = owner read+write, others read only
chmod 600 = rw------- = owner only
-  →  regular file
d  →  directory (folder)
*/

/*
Phase A — Lesson 3
Text Tools

cat /proc/cpuinfo    # print whole file
head -n 5 /proc/cpuinfo     # first 5 lines
tail -n 5 /proc/cpuinfo     # last 5 lines
tail -f can_log.txt
tail -f /home/azad/can_log.txt
grep "error" logfile.txt          # find lines containing "error"
grep -i "error" logfile.txt       # case-insensitive search
grep -n "error" logfile.txt       # show line numbers too
grep -c "error" logfile.txt       # count how many matches
grep -r "error" /var/log/         # search recursively through folders
grep -c "0x300" can_log.txt
find /home/azad -name "*.c"          # find all .c files
find /home/azad -name "can_*"        # find files starting with "can_"
find / -name "vcan*" 2>/dev/null     # find anything related to vcan
find /home/azad        search starting from this folder
-name "*.c"     match files ending in .c
2>/dev/nullhide "permission denied" error messages
wc -l can_log.txt        # count lines
wc -w can_log.txt        # count words
wc -c can_log.txt        # count characters/bytes
wc -l can_log.txt
command1 | command2
grep "0x101" can_log.txt | wc -l
tail -f can_log.txt | grep "0x101"
grep "0x300" can_log.txt | tail -n 3
find ~ -name "*.c" | wc -l
cd ~
ls
# 1. See last 10 lines of your log
tail can_log.txt

# 2. Find all RPM frames
grep "0x101" can_log.txt

# 3. Count how many RPM frames exist
grep "0x101" can_log.txt | wc -l

# 4. Count total lines in the log
wc -l can_log.txt

# 5. Find all your C files
find ~ -name "*.c"

# 6. Combine grep and head — show only first 3 tire pressure readings
grep "0x300" can_log.txt | head -n 3

*/

/*
Lesson 4
Processes

Program on disk        →  just a file (e.g. ./can_logger)
Program running         →  a PROCESS with a PID (Process ID)
ps      See Running Processes
PID TTY          TIME CMD
 1234 pts/0    00:00:00 bash
 5678 pts/0    00:00:00 ps
ColumnMeaningPIDProcess ID — unique numberTTYWhich terminal it's running inTIMECPU time usedCMDThe command/program name

Column                 Meaning
PID                    Process ID — unique number
TTY                    Which terminal it's running in
TIME                   CPU time used
CMD                    The command/program name

USER   PID  %CPU %MEM    VSZ   RSS TTY   STAT START   TIME COMMAND
azad     1   0.0  0.1   1234   800 ?     Ss   10:00   0:00 /sbin/init
azad   245   0.0  0.5   8932  4200 pts/0 S    10:05   0:01 ./can_logger

Flag            Meaning
a               show processes from all
u               sersushow detailed info (CPU%, memory%)
x               include processes without a terminal

ps aux | grep can_logger
top
PID  USER  %CPU  %MEM  TIME+   COMMAND
245  azad   2.3   0.5  0:01.20 can_logger
312  azad   0.1   0.2  0:00.05 bash
Useful keys inside top
q — quit
k — kill a process (it'll ask for PID)
M — sort by memory usage
P — sort by CPU usage

./can_logger &
[1] 4521
jobs — See background jobs in this terminal
Bring a background job back to foreground
fg
kill 4521
kill -9 4521
signal(SIGINT, handle_exit);
kill 4521        → sends SIGTERM (default)
kill -2 4521      → sends SIGINT  (same as Ctrl+C)
kill -9 4521      → sends SIGKILL (force kill, can't be caught)
Ctrl+C            → sends SIGINT directly to foreground process
# 1. Start your CAN logger in the background
cd ~
./can_logger &

# 2. Check it's running
jobs
ps aux | grep can_logger

# 3. Note the PID number, then send some CAN frames
cansend vcan0 101#1111
cansend vcan0 300#2222

# 4. Check the log file is being written even though logger runs in background
tail can_log.txt

# 5. Bring it to foreground
fg
# Press Ctrl+C to stop it normally

# 6. Try again but kill it from another terminal instead
./can_logger &
jobs
# note the PID
kill <PID>     # replace <PID> with the actual number

# 7. Confirm it stopped
jobs
ps aux | grep can_logger

*/
/*
Phase A — Lesson 5
Shell Scripting Basics


touch my_script.sh
echo '#!/bin/bash' > my_script.sh
echo 'echo Hello from my script' >> my_script.sh
chmod 755 my_script.sh
./my_script.sh

#!/bin/bash     "run this script with bash"
#!/bin/bash

# Define variables — no spaces around =
name="Azad"
interface="vcan0"
count=5

# Use variables with $
echo "Hello $name"
echo "Using interface: $interface"
echo "Count is: $count"

#!/bin/bash
echo "Enter your name:"
read username
echo "Welcome $username"

#!/bin/bash

temperature=95

if [ $temperature -gt 100 ]; then
    echo "WARNING: Engine overheating!"
elif [ $temperature -gt 90 ]; then
    echo "Temperature high but normal"
else
    echo "Temperature OK"
fi

Claude finished the responsedotnet/C# + Embedded Linux + CAN Bus developers
What does this mean? what kind of qualifications does it need?Searched the webSearched the webThis is a fairly specialized job profile that sits at the crossroads of three distinct technical domains. Let me break it down clearly.

What the Role Means
dotnet/C# — The application layer is written in C# using .NET (likely .NET Core / .NET 6+ for cross-platform). This is the "business logic" — the part that processes data, exposes APIs, runs services, or manages a UI.
Embedded Linux — The software runs on a Linux-based embedded device (think: industrial controller, automotive ECU, robotics unit, medical device, IoT gateway) — not a desktop PC. This means working with constrained hardware, custom OS builds (Yocto, Buildroot), and low-level system concerns.
CAN Bus — Controller Area Network is a real-time serial communication protocol used extensively in automotive, industrial, and robotics systems. It allows microcontrollers/devices to talk to each other without a host computer. Think: car ECUs, sensor nodes, motor controllers.
Together this describes someone building application-layer software in C#/.NET that runs on an embedded Linux device and communicates with hardware over CAN Bus — common in automotive, industrial automation, and robotics.

Qualifications Typically Required
Core Programming

Strong C# and .NET (ideally .NET 6/8, cross-platform)
Comfort with low-level concerns like memory management, threading, async I/O
Ideally some C/C++ background (common in embedded environments)

Embedded Linux

Working knowledge of Linux internals — processes, IPC, file systems, device drivers
Experience with embedded distros like Yocto or Buildroot
Cross-compilation (building on x86, deploying to ARM)
SSH, systemd, shell scripting

CAN Bus

Understanding of the CAN protocol (framing, arbitration, IDs, bitmasks)
Experience with libraries like SocketCAN (Linux's CAN interface) or CANopen
Familiarity with tools like candump, cansend, or CANalyzer

General Embedded Skills

Serial protocols: SPI, I2C, UART, USB — and now CAN
Real-time constraints and deterministic behavior
Hardware debugging (oscilloscopes, logic analyzers, CAN sniffers)

Education

BSc/MSc in Electrical Engineering, Computer Engineering, or Computer Science
Industry domains: automotive, industrial automation, robotics, medical devices


How Does Your Background Map to This?
Given your thesis on FPGA + TinyML + IMU and your Bosch Sensortec internship (sensor interfaces, I3C protocol), you already have meaningful overlap:
Their RequirementYour BackgroundEmbedded systems mindset✅ Bosch Sensortec, I3C/IMU workCommunication protocols✅ I3C, SPI, MIPI experienceLinux (embedded)Partial — depends on your exposureCAN Bus❌ Likely a gapC# / .NET❌ Likely a gap (you're more C/C++ / hardware side)
If this is a job you're considering, the biggest gaps would be C#/.NET and CAN Bus specifically — but both are learnable, and your embedded foundation is genuinely strong.if i want to learn this, can you help meAbsolutely, I can help you learn this step by step. Before I put together a roadmap, let me ask a couple of things to tailor it properly.Q: What's your current C# / .NET experience?
A: Complete beginnerQ: What's your goal with learning this?
A: Land a job in this fieldQ: How much time can you dedicate per week?
A: 1–3 hrs (slow & steady)Perfect — that's really helpful. Given you're starting from zero on C# but already have a strong embedded background (Bosch, I3C, FPGA, IMU), here's a realistic, structured roadmap built around your pace.

Your Learning Roadmap
dotnet/C# + Embedded Linux + CAN Bus

Phase 1 — C# & .NET Foundations
~2–3 months at 1–3 hrs/week
This is your biggest gap, so it gets the most time. Focus on things relevant to embedded/systems work, not web dev.
Learn:

C# syntax, types, control flow
OOP — classes, interfaces, inheritance
Async/await and threading (critical for real-time embedded apps)
File I/O and working with serial ports (System.IO.Ports)

Resources (free):

cs-fundamentals.com or Microsoft's own C# docs
"C# in a Nutshell" (book) — best reference for systems-oriented C#
dotnet.microsoft.com — official tutorials, run in browser, no setup needed

Mini project: Write a C# console app that reads data from a file, parses it, and prints structured output. Simple but builds muscle memory.

Phase 2 — .NET on Linux
~1–2 months
Since you already know Linux from your embedded work, this phase is mostly about connecting C# to that world.
Learn:

Running .NET on Linux (dotnet run on Ubuntu/Debian)
Cross-compilation — build on x86, deploy to ARM (Raspberry Pi is perfect here)
Calling Linux system APIs from C# (System.Diagnostics.Process, P/Invoke for native calls)
systemd service — running your .NET app as a background daemon

Mini project: Deploy a .NET console app to a Raspberry Pi that reads CPU temperature from /sys/class/thermal/ and logs it every second.

Phase 3 — CAN Bus
~2 months
This builds directly on your protocol knowledge (I3C, SPI). CAN Bus will feel familiar in concept.
Learn:

CAN protocol fundamentals — frames, IDs, arbitration, bitmasks
SocketCAN — Linux's built-in CAN interface (works like a network socket)
Using candump / cansend CLI tools to test
Interfacing SocketCAN from C# using raw sockets or a library like Peak.Can.Basic or SocketCANSharp

Hardware you'll need: A Raspberry Pi + a cheap MCP2515 CAN module (~€10–15) is enough to practice.
Mini project: Send and receive CAN frames between two nodes (even two processes on the same Pi via virtual CAN vcan0).

Phase 4 — Put It All Together
~2–3 months
Build one real portfolio project that combines everything.
Project idea: A C# .NET service running on embedded Linux (Raspberry Pi) that:

Listens to CAN Bus messages from a simulated sensor node
Parses and logs the data
Exposes a simple REST API or dashboard showing live values

This is exactly the kind of thing an employer in automotive/industrial will recognize immediately.

Realistic Timeline
PhaseFocusDuration1C# & .NET basics2–3 months2.NET on Linux1–2 months3CAN Bus + SocketCAN2 months4Portfolio project2–3 monthsTotal~9–12 months
At 1–3 hrs/week this is honest — not rushed. You'd be genuinely job-ready at the end, not just "familiar with."

Your Strengths That Give You a Head Start
Since you already have embedded systems experience, you're skipping a lot of the painful learning curve most people face:

✅ You understand hardware/software boundaries
✅ Serial protocols (I3C, SPI) translate directly to understanding CAN
✅ Linux comfort from your thesis/internship work
✅ C background helps C# feel natural faster


Want me to start with Phase 1 right now? I can give you your first lesson, exercises, and explain things in a way that connects to your existing C/embedded knowledge.i want to learn phase 3 first, can busThat makes total sense — CAN Bus is closest to what you already know from I3C and SPI work at Bosch. Let's dive in.

CAN Bus — Phase 3 Lesson 1
What is CAN Bus and Why Does It Exist?

The Problem It Solves
In a car or industrial machine, you might have 50+ components — engine controller, brake system, dashboard, sensors — all needing to talk to each other. You could wire every component directly to every other component, but that becomes an impossible mess.
CAN Bus solves this with a single shared wire pair that everything connects to. Any node can send a message, and every other node can hear it and decide if it cares.
You've seen this concept before — it's similar to how I3C works on a shared bus, just optimized for longer distances and noisier environments (cars, factories).

Physical Layer — The Wire
CAN uses two wires: CAN_H (high) and CAN_L (low). They carry the same signal as a differential pair — the receiver looks at the voltage difference between them, which makes it very noise-resistant. This is why CAN works reliably in engine bays and factory floors.
Node A          Node B          Node C
  |               |               |
CAN_H ══════════════════════════════
CAN_L ══════════════════════════════
  |               |               |
120Ω                             120Ω  ← termination resistors at each end
Speed goes up to 1 Mbit/s at short distances, typically 500 kbit/s in automotive.

The CAN Frame — How a Message Looks
Every message on CAN Bus is called a frame. The standard frame looks like this:
┌─────────┬─────┬────┬──────────────────┬─────┐
│  SOF    │ ID  │ DLC│      Data        │ CRC │
│  1 bit  │11bit│4bit│   0–8 bytes      │16bit│
└─────────┴─────┴────┴──────────────────┴─────┘

SOF — Start of Frame, just a sync bit
ID — 11-bit identifier (or 29-bit in extended CAN). This is NOT a node address — it identifies the message type
DLC — Data Length Code, how many bytes of data follow (0–8)
Data — The actual payload, up to 8 bytes
CRC — Error checking, like you've seen in I3C


The Key Concept: No Addresses
This is what surprises most people coming from I2C/I3C/SPI:

CAN has no destination address. Every node receives every message. Each node then checks the ID and decides whether to process it or ignore it.

For example:

ID 0x100 might mean "engine RPM"
ID 0x200 might mean "brake pressure"
The dashboard listens for 0x100, the ABS controller listens for 0x200
Both hear both messages, but each only acts on what it cares about


Arbitration — What Happens When Two Nodes Talk at Once?
This is CAN's clever trick. If two nodes transmit simultaneously:

The one with the lower ID wins (lower ID = higher priority)
The loser automatically backs off and retries
No collision, no data corruption, no coordinator needed

This is called non-destructive bitwise arbitration and it's why CAN works without a master node — unlike I2C/I3C where you have a master controlling the bus.

Comparison to What You Already Know
FeatureI3C (your experience)CAN BusTopologyMulti-drop busMulti-drop busWires2 (SDA, SCL)2 (CAN_H, CAN_L)Master/SlaveYes, master controlsNo master — peer to peerAddressingNode addressesMessage IDs (not node addresses)SpeedUp to 12.5 Mbit/sUp to 1 Mbit/sNoise immunityModerateVery high (differential)Use caseSensors on PCBAutomotive, industrial

Where CAN Is Used

Automotive — Every modern car has 1–3 CAN buses. Engine, transmission, ABS, airbags, infotainment all talk over it
Industrial — CANopen protocol runs factory machines, conveyors, robots
Medical — Some medical imaging and surgical equipment
Aerospace — Smaller aircraft and drones


What's Next in Phase 3
Here's the full sequence for CAN Bus:

✅ Lesson 1 — Protocol fundamentals (you're here)
Lesson 2 — SocketCAN on Linux (setup, vcan0, candump, cansend)
Lesson 3 — Reading/writing CAN frames from C# using SocketCANSharp
Lesson 4 — Parsing real data (DBC files, signal decoding)
Lesson 5 — Mini project: CAN data logger in C#


One quick question before Lesson 2 — do you have a Raspberry Pi or any Linux machine available to practice on, or will you be using a virtual machine for now? It'll affect how I set up the SocketCAN lesson.i dont have any rasberry pi and linux machine, but i can use online linuxGood thinking — that works fine for learning SocketCAN. We can use virtual CAN (vcan0) which doesn't need any real hardware at all. It's actually how most developers test CAN software before connecting real devices.
Let me check which online Linux environments support the kernel modules we need for SocketCAN.Searched the webSearched the webGood news — the setup is well documented and fully doable online. Here's the honest picture:

Online Linux — What Works and What Doesn't
The vcan0 virtual CAN interface needs two things from the Linux kernel:

sudo modprobe vcan — to load the kernel module
sudo ip link add dev vcan0 type vcan — to create the virtual interface

Most browser-based Linux terminals (like JSLinux, Replit, or CodeSandbox) run containers without real kernel access, so modprobe won't work there.
The best free option for you: killercoda.com
It gives you a real Ubuntu VM in the browser with full sudo access and kernel module support. It's free, no install needed, and resets each session.

Lesson 2 — SocketCAN Hands-On
Setting Up vcan0 and Sending Your First CAN Frame

Step 1 — Open Your Environment
Go to killercoda.com → choose any Ubuntu scenario → open the terminal. You get a real Linux shell with sudo.

Step 2 — Set Up the Virtual CAN Interface
Run these three commands one by one:
bash# Load the virtual CAN kernel module
sudo modprobe vcan

# Create a virtual CAN interface named vcan0
sudo ip link add dev vcan0 type vcan

# Bring it online (like plugging in a cable)
sudo ip link set up vcan0
Verify it worked:
baship link show vcan0
You should see something like:
3: vcan0: <NOARP,UP,LOWER_UP> mtu 72 qdisc noqueue state UNKNOWN
    link/can
UP and LOWER_UP = it's alive. Think of this like bringing up a network interface — same concept as ifconfig eth0 up but for CAN.

Step 3 — Install can-utils
This is the standard Linux toolkit for CAN — like netcat but for CAN Bus.
bashsudo apt-get install -y can-utils
This gives you two key tools:

candump — listen to all traffic on the bus (like Wireshark for CAN)
cansend — send a CAN frame manually


Step 4 — Send Your First CAN Frame
Open two terminals side by side (killercoda lets you split).
Terminal 1 — start listening:
bashcandump vcan0
It just waits, like a receiver.
Terminal 2 — send a frame:
bashcansend vcan0 0x100#DEADBEEF
Breaking that down:
cansend vcan0   →  send on the vcan0 interface
0x100           →  CAN ID (message type: e.g. "engine RPM")
#               →  separator
DEADBEEF        →  data payload in hex (4 bytes)
Terminal 1 should immediately show:
vcan0  100   [4]  DE AD BE EF
That's a real CAN frame being sent and received — on a virtual bus, no hardware needed.

Step 5 — Understand What You Just Saw
vcan0   100    [4]    DE AD BE EF
  │      │      │         │
  │      │      │         └── data payload (4 bytes)
  │      │      └──────────── DLC: data length
  │      └─────────────────── CAN ID (hex): message type
  └────────────────────────── interface name
Try sending a few more frames with different IDs and data:
bashcansend vcan0 0x200#0102030405060708   # 8 bytes, ID 0x200
cansend vcan0 0x001#FF                 # 1 byte, highest priority (low ID)
cansend vcan0 0x7FF#00                 # lowest priority (high ID)
Watch how candump catches all of them — remember, every node hears every frame.

What You've Learned So Far
ConceptHow You Saw ItVirtual CAN busvcan0 — no hardware neededSending a framecansend vcan0 ID#DATAReceiving framescandump vcan0 — catches everythingCAN ID as priorityLower ID = higher priorityNo addressingAll frames go to all listeners

Next Lesson Preview
Lesson 3 is where it gets exciting for your job goal — we'll read and write CAN frames from code, not just CLI tools. We'll use a small C program first (since you know C from embedded work), then move toward the C# approach.
Go try the steps above on killercoda and let me know what output you get — or if you hit any errors, paste them here and we'll fix them together.i dont get this point 'choose any Ubuntu scenario'No worries! Let me walk you through it exactly.

Go to 👉 killercoda.com/scenarios
You'll see a page with many scenario cards. Just pick the one that says:

"Ubuntu Playground" or "Ubuntu 22.04"

It looks like this — just a blank Ubuntu terminal with no specific topic, which is exactly what we want.github.comdev.togithub.comResults from the webHere's the exact step-by-step:

1. Go to: https://killercoda.com/playgrounds
2. Click on "Ubuntu" (the plain one, not Kubernetes or anything else)
3. Click "Start" — it takes about 30 seconds to load
4. You'll see a black terminal on the right side of the screen — that's your real Linux shell
5. You might need to create a free account (just email + password, takes 1 minute)

Once you see the black terminal and a prompt like this:
root@ubuntu:~$
You're in. That's when you start typing the commands from Lesson 2.

Try that and let me know when you see the root@ubuntu:~$ prompt — then we continue from Step 2 (the modprobe vcan commands).i got this after write first command 'sudo modprobe vcan'
Module vcan not found in the directoryThat's a common issue on some cloud VMs — the vcan kernel module isn't pre-installed. Let's check what kernel version you have and work around it.
Run this and tell me what it shows:
bashuname -r
Then also run:
bashfind /lib/modules -name "vcan*" 2>/dev/null
Paste the output here. That tells us whether the module exists but is just in a different path, or if it's genuinely missing from this kernel build.

Meanwhile — here's a backup plan:
If the module is truly missing on killercoda, we switch to a better alternative: Google Cloud Shell
It's completely free, no signup beyond a Google account, gives you a real Debian VM with persistent storage, and importantly — it has full kernel module support including vcan.
Just go to shell.cloud.google.com, log in with your Google account, and you get a terminal immediately. No setup, no time limit per session.
Try the uname -r command first though — paste the result and I'll tell you exactly what to do next.6.8.0-124-genericThat's actually a modern kernel — vcan should be available. The issue is just that killercoda's container doesn't have the full module directory. Let's try this:
bashfind /lib/modules/6.8.0-124-generic -name "vcan*" 2>/dev/null
Paste what you get. My guess is it returns nothing, which confirms the module files are simply not there in this container.

In that case, just switch to Google Cloud Shell — it'll work properly.

Go to 👉 shell.cloud.google.com
Log in with your Google account
Wait ~20 seconds for the terminal to appear
You'll see a prompt like:

yourname@cloudshell:~$
Then run the same first command again:
bashsudo modprobe vcan
This time it should return nothing — which means success (no output = module loaded fine in Linux).
Then continue with:
bashsudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0
ip link show vcan0
Go try it and paste whatever output you see — even if it's an error. We'll get it working.mistazad@cloudshell:~$ sudo modprobe vcan
modprobe: FATAL: Module vcan not found in directory /lib/modules/6.6.137+This is a known limitation of cloud shell environments — they run stripped-down kernels without extra modules like vcan. No worries though, there's a clean workaround.

Solution — Use a Docker Container
Google Cloud Shell has Docker available. We can spin up a full Ubuntu container that shares the host network, which gives us vcan0 access.
Try this:
bashdocker run -it --privileged --network host ubuntu:22.04 bash

--privileged gives full kernel access inside the container
--network host shares the network interfaces with the host

Once you're inside (prompt changes to root@...#), run:
bashapt-get update -qq && apt-get install -y kmod iproute2 can-utils
modprobe vcan
ip link add dev vcan0 type vcan
ip link set up vcan0
ip link show vcan0
Paste the output of ip link show vcan0 and let me know what you see.

If Docker also fails with vcan, the cleanest solution for you would honestly be to install WSL2 on your Windows PC (if you have one). It takes 5 minutes, it's free, and it gives you a full Ubuntu environment with proper kernel modules — much better than any cloud option for this kind of low-level kernel work.
Do you have a Windows machine available?What does it mean by sudo, modprobe, ip link, apt-get, candumpGreat question — these are all fundamental Linux commands you'll use constantly as an embedded Linux developer. Let me explain each one clearly.

Core Linux Commands Explained

sudo
"Superuser Do"
Runs a command as administrator (root). Linux protects sensitive operations — loading kernel modules, changing network interfaces — behind a permission wall. sudo unlocks that.
bashsudo modprobe vcan        # run modprobe as admin
sudo apt-get install ...  # install software as admin
Think of it like "Run as Administrator" in Windows. Without sudo, sensitive commands just get rejected with "Permission denied."

modprobe
"Module Probe"
Linux kernel features are split into modules — like plugins that can be loaded/unloaded without rebooting. modprobe loads one of these modules into the running kernel.
bashsudo modprobe vcan    # load the virtual CAN module into the kernel
In your embedded work context — when you connect a new sensor to Linux, its driver is a kernel module. modprobe is how you activate it. It's similar in concept to loading a firmware blob onto an FPGA.

ip link
Network Interface Manager
ip link manages network interfaces — turning them on/off, creating virtual ones, showing their status. In Linux, CAN Bus interfaces are treated like network interfaces, which is the whole idea behind SocketCAN.
baship link show                        # list all interfaces
ip link add dev vcan0 type vcan     # create a new virtual CAN interface
ip link set up vcan0                # turn it on (like plugging in a cable)
You might know ifconfig — ip link is its modern replacement. Same concept.

apt-get
Package Manager
Ubuntu's tool for installing software from the internet. Like an app store but for the terminal.
bashsudo apt-get update              # refresh the list of available software
sudo apt-get install can-utils   # download and install can-utils
can-utils is the package that gives you candump, cansend, and other CAN tools. One command installs everything.

candump
CAN Bus Monitor
Part of the can-utils package. It listens on a CAN interface and prints every frame it sees — like Wireshark but for CAN Bus.
bashcandump vcan0       # monitor all CAN traffic on vcan0
Output looks like:
vcan0  100   [4]  DE AD BE EF
vcan0  200   [2]  0A FF
It just keeps running and printing until you press Ctrl+C. In real automotive/industrial work, engineers use this to debug what's happening on the bus — which ECU is sending what, at what rate.

How They Connect Together
Here's the full flow we're doing, now with context:
sudo modprobe vcan
│
└── "Hey kernel, load the virtual CAN plugin"

sudo ip link add dev vcan0 type vcan
│
└── "Create a virtual CAN interface called vcan0"

sudo ip link set up vcan0
│
└── "Turn vcan0 on"

sudo apt-get install can-utils
│
└── "Download candump, cansend and other tools"

candump vcan0
│
└── "Start listening to everything on vcan0"

cansend vcan0 0x100#DEADBEEF
│
└── "Send a CAN frame with ID 0x100 and data DEADBEEF"

Now that you know what each command does, the steps from Lesson 2 will make much more sense when you run them.
Shall we get back to fixing the vcan module issue — do you have a Windows PC we can use WSL2 on? That'll be the most reliable environment for this.yes i have windows pc, windows 11Perfect — WSL2 on Windows 11 is actually the best possible setup for this. It's a real Linux kernel running inside Windows, full module support, and you can keep files between sessions unlike cloud shells.

Setting Up WSL2 on Windows 11
Step 1 — Open PowerShell as Administrator

Press Windows key
Type PowerShell
Right-click it → "Run as administrator"


Step 2 — Install WSL2 with Ubuntu
Type this single command and press Enter:
powershellwsl --install
This automatically:

Enables WSL2
Downloads Ubuntu 22.04
Sets everything up

It will ask you to restart your PC at the end. Do that.

Step 3 — After Restart
Ubuntu will open automatically and ask you to create a username and password:
Enter new UNIX username: azad
New password: (type anything, it won't show on screen)
Use whatever username/password you like. This is just for your local Linux.

Step 4 — Test It Worked
Once you see the prompt like:
azad@DESKTOP:~$
Run:
bashuname -r
You should see something like 5.15.x-microsoft-standard-WSL2 — that's the real WSL2 kernel.

Step 5 — Test vcan
bashsudo modprobe vcan
This time it should return nothing — which means success.

Try these steps and paste whatever you see after Step 4 and 5. We'll continue from there.If Ubuntu opens automatically how to use windowsGood practical question! WSL2 runs alongside Windows — it doesn't replace it. You can use both at the same time.

How It Works
WSL2 is just an app running in the background. Windows stays completely normal.

Opening and Closing Ubuntu
To open Ubuntu anytime:

Press Windows key → type Ubuntu → press Enter
A terminal window opens — that's your Linux

To close Ubuntu:

Just close the window like any app (X button)
Or type exit in the terminal
Windows keeps running normally, nothing changes


They Run Side by Side
Your Windows 11 PC
├── Chrome, Word, games, everything normal   ← Windows side
└── Ubuntu terminal window                   ← Linux side (WSL2)
    (open it when you need it, close when done)
Think of Ubuntu like opening Notepad or Calculator — it's just another window. Your Windows desktop, files, and apps are completely untouched.

Bonus — You Can Access Windows Files from Ubuntu
Your Windows files are visible inside Ubuntu:
bashcd /mnt/c/Users/YourName/Desktop
ls
And from Windows side, you can open Ubuntu files in File Explorer by typing \\wsl$ in the address bar.

So the workflow will be:

Use Windows normally for everything else
When you want to practice CAN Bus → open Ubuntu terminal
Done practicing → close the terminal, back to Windows

Go ahead and run the WSL2 install when ready. Let me know what happens after restart!after restart, ubuntu did not open automaticallyNo problem — that sometimes happens. Just open it manually:

Press Windows key → type Ubuntu → press Enter
It should appear in the Start menu like this:
Alternatively try:
Windows key → type wsl → press Enter

If Ubuntu opens and asks you to set a username/password — great, do that and paste the prompt you see after.
If it opens and shows an error message — paste the exact error here.
If Ubuntu doesn't appear in the Start menu at all, run this in PowerShell (as administrator):
powershellwsl --list
Paste what it shows and we'll fix it from there.is it realted to ubuntu 2026-05 .NET Framework PreviewNo, that's not related — ignore that. That's a .NET preview package that happens to show up in search results, nothing to do with what we're setting up.

What you're looking for is simply called "Ubuntu" in the Start menu — it's a plain terminal app, no .NET, no framework, just Linux.
Try this exact sequence:

Press Windows key
Type exactly: ubuntu
Look for this in the results:

Ubuntu
App
It should have a plain orange circle logo with a white Ubuntu symbol.

If you don't see it, try opening it a different way:

Press Windows key + R
Type ubuntu and press Enter


Or from PowerShell (as administrator), just type:
powershellubuntu
That directly launches it.

Tell me what you see when you try any of these — does Ubuntu appear, does it show an error, or is it completely missing?its working nowGreat! Now let's verify everything is set up correctly.
Run these two commands one by one and paste what you see:
bashuname -r
Then:
bashsudo modprobe vcan
For the sudo command it will ask for your password — type the one you created during Ubuntu setup and press Enter (the password won't show on screen while typing, that's normal).doneWhat did the output show? Paste it here so I can confirm:

What uname -r printed (the kernel version)
Whether sudo modprobe vcan returned anything or was silent

If modprobe vcan was silent (no output, just returned to the prompt) — that's actually success! We can move straight to the next step.6.18.33.2-microsoft-standard-WSL2That's a perfect WSL2 kernel — modern and full featured. And if sudo modprobe vcan was silent, that means vcan loaded successfully!
Now run these commands one by one:
bashsudo ip link add dev vcan0 type vcan
bashsudo ip link set up vcan0
baship link show vcan0
Paste the output of the last command and let's see if your virtual CAN bus is alive.doneAgain — what did it show? Paste the output of ip link show vcan0 here.
If it worked, you should see something like:
3: vcan0: <NOARP,UP,LOWER_UP> mtu 72 qdisc noqueue state UNKNOWN
    link/can
The key things to look for are UP and LOWER_UP in that first line — that means your virtual CAN bus is running.3: vcan0: <NOARP,UP,LOWER_UP> mtu 2060 qdisc noqueue state UNKNOWN mode DEFAULT group default qlen 1000
    link/canThat's perfect! Your virtual CAN bus is alive and running. 🎉
UP and LOWER_UP = vcan0 is online and ready.

Now the fun part — sending your first real CAN frame.
Install can-utils first:
bashsudo apt-get install -y can-utils
Wait for it to finish, then we'll open two terminals side by side.

Two terminals in WSL2
You need two Ubuntu windows open at the same time:

Window 1 → listener (candump)
Window 2 → sender (cansend)

To open a second Ubuntu terminal:

Press Windows key → type Ubuntu → press Enter
A second window opens, both connected to the same WSL2 instance


Once can-utils is installed and you have two terminals open, tell me and I'll give you the exact commands to run in each window.mista@DESKTOP-EU8OFN9:~$ sudo apt-get install -y can-utils
Reading package lists... Done
Building dependency tree... Done
Reading state information... Done
E: Unable to locate package can-utilsNo problem — just need to update the package list first. Run this:
bashsudo apt-get update
Wait for it to finish (it downloads the latest package list), then try again:
bashsudo apt-get install -y can-utils
Paste what you see after both commands.its workingExcellent! Now the fun part — sending your first CAN frame.

Open a second Ubuntu terminal

Press Windows key → type Ubuntu → Enter
A second black window opens

You should now have two Ubuntu windows side by side.

Window 1 — Start Listening
In the first Ubuntu window, run:
bashcandump vcan0
It will just sit there waiting — that's correct. It's now listening to everything on the CAN bus.

Window 2 — Send a CAN Frame
In the second Ubuntu window, run:
bashcansend vcan0 0x100#DEADBEEF

The moment you press Enter in Window 2, Window 1 should immediately print:
vcan0  100   [4]  DE AD BE EF
Tell me what you see!cansend vcan0 0x100#DEADBEEF
Wrong CAN-frame format!
cansend - send CAN-frames via CAN_RAW sockets.
Usage: cansend <device> <can_frame>.
<can_frame>:
 <can_id>#{data}          for Classical CAN 2.0 data frames
 <can_id>#R{len}          for Classical CAN 2.0 data frames
 <can_id>#{data}_{dlc}    for Classical CAN 2.0 data frames
 <can_id>#R{len}_{dlc}    for Classical CAN 2.0 data frames
 <can_id>##<flags>{data}  for CAN FD frames
<can_id>:
 3 (SFF) or 8 (EFF) hex chars
{data}:
 0..8 (0..64 CAN FD) ASCII hex-values (optionally separated by '.')
{len}:
 an optional 0..8 value as RTR frames can contain a valid dlc field
_{dlc}:
 an optional 9..F data length code value when payload length is 8
<flags>:
 a single ASCII Hex value (0 .. F) which defines canfd_frame.flags
Examples:
  5A1#11.2233.44556677.88 / 123#DEADBEEF / 5AA# / 123##1 / 213##311223344 /
  1F334455#1122334455667788_B / 123#R / 00000123#R3 / 333#R8_EShow moreEasy fix — the CAN ID format needs to be without 0x. Try:
bashcansend vcan0 100#DEADBEEF
Just 100 instead of 0x100. The cansend tool expects plain hex without the 0x prefix.workingLet's go! 🎉
What do you see in Window 1 (the candump window)? It should show something like:
vcan0  100   [4]  DE AD BE EFyesYou just sent and received your first CAN frame! That's Lesson 2 complete.
Let's make sure you understand what just happened end to end:
Window 2 (you typed)          Window 1 (candump showed)
─────────────────────         ─────────────────────────
cansend vcan0 100#DEADBEEF → vcan0  100  [4]  DE AD BE EF
                                      │    │       │
                                      │    │       └── your data payload
                                      │    └────────── 4 bytes long
                                      └─────────────── CAN ID 0x100

Now try these yourself in Window 2 and watch Window 1:
bashcansend vcan0 200#0102030405060708
bashcansend vcan0 001#FF
bashcansend vcan0 7FF#00
Notice how 001 appears before 7FF in the dump — lower ID = higher priority, exactly like we covered in Lesson 1.

What You've Accomplished So Far
LessonTopicStatus1CAN Bus fundamentals✅ Done2SocketCAN + vcan0 + candump/cansend✅ Done3Reading/writing CAN frames from code⬅️ Next

Lesson 3 is where it gets really interesting — we'll write actual C code that sends and receives CAN frames using Linux sockets. Since you already know C from your embedded background, it'll feel familiar.
Ready to continue?how to send all 3 commands at same timeGood thinking! Two ways:

Option 1 — Semicolon (run one after another)
bashcansend vcan0 200#0102030405060708 ; cansend vcan0 001#FF ; cansend vcan0 7FF#00
They run sequentially, one after the other very fast.

Option 2 — Script (best way)
bashfor id in 200#0102030405060708 001#FF 7FF#00; do cansend vcan0 $id; done
Same result, but more elegant and scalable — you can add as many frames as you want.

Both will make candump in Window 1 print all three frames almost instantly. Try Option 1 first since it's simpler to read.it did not happen like this way Notice how 001 appears before 7FF in the dump — lower ID = higher priority,Good observation — and this is actually correct behavior! Don't worry, you didn't do anything wrong.

Why This Happens on vcan0
On a real CAN bus with actual hardware, two nodes transmitting simultaneously would trigger arbitration — the lower ID wins and appears first.
But on vcan0 (virtual CAN), you're sending frames sequentially from one terminal, not simultaneously from multiple nodes. So they just appear in the order you sent them:
200 → 001 → 7FF
No arbitration happens because there's no collision — you're the only sender.

To Actually See Priority in Action
You would need two separate programs trying to transmit at the exact same moment on real hardware. That's when the lower ID physically wins the bus.
On vcan0 you can't simulate true simultaneous collision — it's a software virtual bus, not real electrical signals.

The Key Takeaway
EnvironmentArbitrationReal CAN hardware✅ Lower ID wins on collisionvcan0 (virtual)❌ Sequential, no collision possible
So what you saw is completely correct for vcan0. The priority concept is real — just can't be demonstrated without real hardware.
Good catch though — that's exactly the kind of critical thinking embedded developers need. Ready for Lesson 3?yes, go for l3Lesson 3 — Reading & Writing CAN Frames from C Code

Why C First, Not C# Yet?
SocketCAN is a Linux kernel interface — at its core it's just Linux sockets. C talks to it natively. Once you understand it in C, the C# version is just a wrapper around the same concept. So C first = solid foundation.

The Concept — SocketCAN is Just a Socket
You've probably heard of network sockets (TCP/IP). SocketCAN works the exact same way — Linux treats CAN like a network interface on purpose.
Normal network socket:        SocketCAN socket:
socket(AF_INET, ...)    →     socket(AF_CAN, ...)
bind to IP + port       →     bind to vcan0 interface
send/recv bytes         →     send/recv CAN frames
Same API, different address family. That's the elegance of SocketCAN.

Your First C Program — CAN Sender
Open a new file in Ubuntu:
bashnano can_send.c
Type or paste this code:
c#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>

int main() {
    // 1. Create a CAN socket
    int sock = socket(AF_CAN, SOCK_RAW, CAN_RAW);
    if (sock < 0) {
        perror("Socket error");
        return 1;
    }

    // 2. Bind to vcan0 interface
    struct ifreq ifr;
    strcpy(ifr.ifr_name, "vcan0");
    ioctl(sock, SIOCGIFINDEX, &ifr);

    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    bind(sock, (struct sockaddr *)&addr, sizeof(addr));

    // 3. Build a CAN frame
    struct can_frame frame;
    memset(&frame, 0, sizeof(frame));
    frame.can_id  = 0x100;           // CAN ID
    frame.can_dlc = 4;               // 4 bytes of data
    frame.data[0] = 0xDE;
    frame.data[1] = 0xAD;
    frame.data[2] = 0xBE;
    frame.data[3] = 0xEF;

    // 4. Send the frame
    write(sock, &frame, sizeof(frame));
    printf("CAN frame sent! ID: 0x%X  Data: %02X %02X %02X %02X\n",
           frame.can_id,
           frame.data[0], frame.data[1],
           frame.data[2], frame.data[3]);

    close(sock);
    return 0;
}
Save and exit: Ctrl+O → Enter → Ctrl+X

Compile and Run
bashgcc can_send.c -o can_send
Make sure candump vcan0 is running in Window 1, then in Window 2:
bash./can_send
Window 2 should print:
CAN frame sent! ID: 0x100  Data: DE AD BE EF
Window 1 (candump) should show:
vcan0  100   [4]  DE AD BE EF

Understanding the Code — Key Parts
csocket(AF_CAN, SOCK_RAW, CAN_RAW)
Creates a raw CAN socket — AF_CAN is the CAN address family, same idea as AF_INET for TCP/IP.
cstruct ifreq ifr;
strcpy(ifr.ifr_name, "vcan0");
ioctl(sock, SIOCGIFINDEX, &ifr);
Looks up the index number of vcan0 — Linux identifies interfaces by number internally, not by name.
cstruct can_frame frame;
frame.can_id  = 0x100;
frame.can_dlc = 4;
frame.data[0] = 0xDE;
This is the actual CAN frame struct from the Linux kernel — can_id, can_dlc (data length), and data[] array. Exactly matches the frame structure from Lesson 1.
cwrite(sock, &frame, sizeof(frame));
Sends the frame — same write() call you'd use for any file or socket in Linux.

Now Write the Receiver
bashnano can_recv.c
c#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>

int main() {
    // 1. Create socket and bind (same as sender)
    int sock = socket(AF_CAN, SOCK_RAW, CAN_RAW);

    struct ifreq ifr;
    strcpy(ifr.ifr_name, "vcan0");
    ioctl(sock, SIOCGIFINDEX, &ifr);

    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(sock, (struct sockaddr *)&addr, sizeof(addr));

    // 2. Wait and receive frames in a loop
    struct can_frame frame;
    printf("Listening on vcan0...\n");

    while (1) {
        int nbytes = read(sock, &frame, sizeof(frame));
        if (nbytes < 0) {
            perror("Read error");
            break;
        }

        printf("Received: ID=0x%X  DLC=%d  Data=",
               frame.can_id, frame.can_dlc);

        for (int i = 0; i < frame.can_dlc; i++) {
            printf("%02X ", frame.data[i]);
        }
        printf("\n");
    }

    close(sock);
    return 0;
}
Save and compile:
bashgcc can_recv.c -o can_recv

Test Sender + Receiver Together
Window 1 — start receiver:
bash./can_recv
Window 2 — send a frame:
bash./can_send
Window 1 should print:
Listening on vcan0...
Received: ID=0x100  DLC=4  Data=DE AD BE EF

What You've Built
can_send.c                    can_recv.c
──────────                    ──────────
Create socket                 Create socket
Bind to vcan0      →vcan0→   Bind to vcan0
Build CAN frame               Wait for frame (read loop)
Write to socket               Print ID + data
Two separate programs communicating over a virtual CAN bus — exactly how real ECUs talk to each other in a car.

Try it and paste what you see in both windows. Once this works, Lesson 4 is about filtering — making your receiver only listen to specific CAN IDs, which is how real nodes ignore messages they don't care about.can you explain me  every section of this code(can_send.c),Sure! Let me walk through every single line.

can_send.c — Full Explanation

Section 1 — Header Files
c#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
These are like import statements — they load pre-written code you need.
HeaderWhat it gives youstdio.hprintf() — printing to terminalstring.hstrcpy(), memset() — string/memory toolsunistd.hwrite(), close() — file/socket operationsnet/if.hstruct ifreq — network interface structsys/socket.hsocket(), bind() — socket functionssys/ioctl.hioctl() — device control callslinux/can.hstruct can_frame, AF_CAN — CAN definitionslinux/can/raw.hCAN_RAW — raw CAN socket type
The last two (linux/can.h and linux/can/raw.h) are specific to SocketCAN — they're part of the Linux kernel headers.

Section 2 — Create the Socket
cint sock = socket(AF_CAN, SOCK_RAW, CAN_RAW);
if (sock < 0) {
    perror("Socket error");
    return 1;
}
socket() creates a communication endpoint and returns a number (sock) that represents it — like opening a file returns a file handle.
The three arguments tell Linux what kind of socket:
ArgumentMeaningAF_CANAddress Family = CAN (not internet, not bluetooth — CAN)SOCK_RAWRaw socket — we handle the full frame ourselvesCAN_RAWRaw CAN protocol specifically
if (sock < 0) — if something went wrong, Linux returns a negative number. perror() prints the error reason and we exit.
Analogy: This is like picking up a telephone handset. You have a connection tool but haven't dialed anyone yet.

Section 3 — Find the Interface Index
cstruct ifreq ifr;
strcpy(ifr.ifr_name, "vcan0");
ioctl(sock, SIOCGIFINDEX, &ifr);
Linux identifies network interfaces internally by number, not by name. This section converts the name "vcan0" into its internal index number.

struct ifreq ifr — a struct that holds interface information
strcpy(ifr.ifr_name, "vcan0") — put the name "vcan0" into it
ioctl(sock, SIOCGIFINDEX, &ifr) — ask the kernel "what is the index number of vcan0?" The answer gets stored back into ifr

ioctl (Input/Output Control) is a general Linux system call for device-specific operations — you'll see this a lot in embedded Linux work.
Analogy: You know the name of the street you want, and you're asking Google Maps for the exact coordinates.

Section 4 — Bind to vcan0
cstruct sockaddr_can addr;
memset(&addr, 0, sizeof(addr));
addr.can_family = AF_CAN;
addr.can_ifindex = ifr.ifr_ifindex;

bind(sock, (struct sockaddr *)&addr, sizeof(addr));
Now we attach the socket to the actual vcan0 interface.

struct sockaddr_can addr — a struct that holds the CAN address (which interface to use)
memset(&addr, 0, sizeof(addr)) — zero out the struct first (good practice, avoids garbage values)
addr.can_family = AF_CAN — tell it this is a CAN address
addr.can_ifindex = ifr.ifr_ifindex — set the interface to vcan0's index number we just got
bind(sock, ...) — attach the socket to that interface

Analogy: You picked up the handset (socket), found the address (ifindex), and now you're plugging the phone into the wall socket (bind). Now it's connected to the CAN bus.

Section 5 — Build the CAN Frame
cstruct can_frame frame;
memset(&frame, 0, sizeof(frame));
frame.can_id  = 0x100;
frame.can_dlc = 4;
frame.data[0] = 0xDE;
frame.data[1] = 0xAD;
frame.data[2] = 0xBE;
frame.data[3] = 0xEF;
This builds the actual CAN frame in memory.

struct can_frame frame — this struct is defined in linux/can.h, it represents one CAN frame
memset(&frame, 0, sizeof(frame)) — zero everything out first
frame.can_id = 0x100 — set the CAN ID (message type identifier)
frame.can_dlc = 4 — Data Length Code = 4 bytes of data
frame.data[0..3] — the actual payload bytes: DE AD BE EF

The can_frame struct looks like this internally:
cstruct can_frame {
    canid_t can_id;    // 32-bit CAN ID
    uint8_t can_dlc;   // data length (0-8)
    uint8_t data[8];   // payload
};
This maps directly to what you learned in Lesson 1 — ID, DLC, Data.
Analogy: You're writing a letter — putting the recipient ID on the envelope and the message inside.

Section 6 — Send the Frame
cwrite(sock, &frame, sizeof(frame));
printf("CAN frame sent! ID: 0x%X  Data: %02X %02X %02X %02X\n",
       frame.can_id,
       frame.data[0], frame.data[1],
       frame.data[2], frame.data[3]);

write(sock, &frame, sizeof(frame)) — sends the frame onto the CAN bus through the socket. Same write() you'd use to write to a file in Linux — everything is a file descriptor in Linux.
printf(...) — prints confirmation to the terminal

%02X means: print as 2-digit uppercase hex, pad with zero if needed. So 0xDE prints as DE not just E.
Analogy: You drop the letter in the postbox. Every node on the bus receives it, each decides if ID 0x100 is relevant to them.

Section 7 — Close the Socket
cclose(sock);
return 0;

close(sock) — releases the socket, frees the resource
return 0 — program exits successfully (0 = success in C)

Always close sockets when done — same as closing a file. In long-running embedded applications this matters a lot.

The Full Picture
socket()     →  Create communication endpoint
ioctl()      →  Find vcan0's internal index number
bind()       →  Attach socket to vcan0
can_frame    →  Build the message (ID + DLC + data)
write()      →  Send it onto the bus
close()      →  Clean up
Every CAN program you write — sender, receiver, logger, filter — follows this same skeleton. The only part that changes is what you do between bind() and close().

Ready to go through can_recv.c the same way, or do you want to try running both programs first?ready to go next codecan_recv.c — Full Explanation

Section 1 — Header Files
c#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
Exact same headers as can_send.c — because the socket setup process is identical for both sender and receiver. The difference comes later in what you do with the socket.

Section 2 — Create Socket and Bind
cint sock = socket(AF_CAN, SOCK_RAW, CAN_RAW);

struct ifreq ifr;
strcpy(ifr.ifr_name, "vcan0");
ioctl(sock, SIOCGIFINDEX, &ifr);

struct sockaddr_can addr;
memset(&addr, 0, sizeof(addr));
addr.can_family = AF_CAN;
addr.can_ifindex = ifr.ifr_ifindex;
bind(sock, (struct sockaddr *)&addr, sizeof(addr));
This is identical to can_send.c — create a socket, find vcan0's index, bind to it.
This is an important point about SocketCAN:

Sender and receiver use the exact same setup. The difference is only write() vs read().

Analogy: Both a radio transmitter and receiver need to be tuned to the same frequency first. Tuning = socket + bind. Then one transmits, one listens.

Section 3 — Prepare to Receive
cstruct can_frame frame;
printf("Listening on vcan0...\n");

struct can_frame frame — empty frame struct, ready to be filled by incoming data
printf(...) — just tells the user the program is running and waiting

Nothing special here — just preparation before the main loop.

Section 4 — The Receive Loop
cwhile (1) {
    int nbytes = read(sock, &frame, sizeof(frame));
    if (nbytes < 0) {
        perror("Read error");
        break;
    }

    printf("Received: ID=0x%X  DLC=%d  Data=",
           frame.can_id, frame.can_dlc);

    for (int i = 0; i < frame.can_dlc; i++) {
        printf("%02X ", frame.data[i]);
    }
    printf("\n");
}
This is the heart of the receiver. Let's break it down piece by piece:

while (1)
cwhile (1) {
An infinite loop — runs forever until you press Ctrl+C or an error occurs. This is standard in embedded systems — a sensor listener or ECU never just reads one message and quits, it runs continuously.

read() — Waiting for a Frame
cint nbytes = read(sock, &frame, sizeof(frame));

read(sock, ...) — reads from the CAN socket, blocks here until a frame arrives
&frame — puts the received data directly into the frame struct
sizeof(frame) — tells Linux exactly how many bytes to read
nbytes — how many bytes were actually received

The key word is blocks — the program pauses at this line, doing nothing, until a CAN frame arrives. The moment a frame comes in, it fills the frame struct and continues.
Analogy: Like candump sitting and waiting — it does nothing until it sees traffic on the bus.

Error Check
cif (nbytes < 0) {
    perror("Read error");
    break;
}
If read() returns negative, something went wrong — interface went down, socket closed, etc. perror() prints the reason, break exits the while loop cleanly.

Print the CAN ID and DLC
cprintf("Received: ID=0x%X  DLC=%d  Data=",
       frame.can_id, frame.can_dlc);

frame.can_id — the CAN ID of the received frame, printed as hex (0x%X)
frame.can_dlc — how many data bytes arrived, printed as decimal (%d)

At this point output looks like:
Received: ID=0x100  DLC=4  Data=
No newline yet — the data bytes come next.

Print the Data Bytes
cfor (int i = 0; i < frame.can_dlc; i++) {
    printf("%02X ", frame.data[i]);
}
printf("\n");

Loop runs frame.can_dlc times — exactly as many bytes as were received (0–8)
frame.data[i] — each byte of the payload
%02X — print as 2-digit hex, uppercase
printf("\n") — newline after all bytes are printed

So for DEADBEEF the output becomes:
Received: ID=0x100  DLC=4  Data=DE AD BE EF

Section 5 — Close
cclose(sock);
return 0;
Same as sender — clean up the socket when done. In practice this code only reaches here if break was called from the error check, since while(1) runs forever otherwise.

Side by Side — Sender vs Receiver
can_send.c                    can_recv.c
──────────────────────────    ──────────────────────────
socket()                      socket()
ioctl() + bind()              ioctl() + bind()

build can_frame               while(1) {
write(sock, &frame)               read(sock, &frame)  ← blocks
printf confirmation               printf ID+data
close()                       }
                              close()
The only real difference is write() vs read() inside a loop.

Key Concepts from This Code
ConceptWhere You Saw ItBlocking readread() pauses until a frame arrivesInfinite loopwhile(1) — standard in embedded receiversSame socket setupSender and receiver both do socket+bindDLC drives the loopfor i < frame.can_dlc — only read actual bytesError handlingCheck nbytes < 0 after every read

Now Run Both Together
Window 1 — start receiver:
bash./can_recv
Window 2 — send frames:
bash./can_send
Then try sending multiple frames rapidly:
bash./can_send ; ./can_send ; ./can_send
Watch Window 1 catch all three. Then try sending from cansend too:
bashcansend vcan0 200#AABBCCDD
Your can_recv should catch that too — because it listens to all IDs, not just 0x100.

Try it and let me know what you see. After this, Lesson 4 is about filtering — making the receiver only listen to specific CAN IDs, ignoring everything else. That's how real ECUs work.give me details unstruction for both codes, how to open file, save, exit, and then run and observStep by Step — Both Files

PART 1 — Create and Run can_send.c

Step 1 — Open a new file
In Ubuntu terminal type:
bashnano can_send.c
A black editor opens with empty space.

Step 2 — Paste the code
Copy this entire code:
c#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>

int main() {
    int sock = socket(AF_CAN, SOCK_RAW, CAN_RAW);
    if (sock < 0) {
        perror("Socket error");
        return 1;
    }

    struct ifreq ifr;
    strcpy(ifr.ifr_name, "vcan0");
    ioctl(sock, SIOCGIFINDEX, &ifr);

    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(sock, (struct sockaddr *)&addr, sizeof(addr));

    struct can_frame frame;
    memset(&frame, 0, sizeof(frame));
    frame.can_id  = 0x100;
    frame.can_dlc = 4;
    frame.data[0] = 0xDE;
    frame.data[1] = 0xAD;
    frame.data[2] = 0xBE;
    frame.data[3] = 0xEF;

    write(sock, &frame, sizeof(frame));
    printf("CAN frame sent! ID: 0x%X  Data: %02X %02X %02X %02X\n",
           frame.can_id,
           frame.data[0], frame.data[1],
           frame.data[2], frame.data[3]);

    close(sock);
    return 0;
}
In the nano editor:

Right click inside the terminal → Paste
Or press Ctrl+Shift+V


Step 3 — Save the file
Press Ctrl+O → press Enter
You'll see at the bottom:
File Name to Write: can_send.c
That confirms it saved.

Step 4 — Exit nano
Press Ctrl+X
You're back to the normal terminal prompt.

Step 5 — Compile
bashgcc can_send.c -o can_send
No output = success. If you see errors, paste them here.


PART 2 — Create and Run can_recv.c

Step 1 — Open a second Ubuntu window
Press Windows key → type Ubuntu → Enter
A second terminal opens.

Step 2 — Open new file
bashnano can_recv.c

Step 3 — Paste the code
c#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>

int main() {
    int sock = socket(AF_CAN, SOCK_RAW, CAN_RAW);

    struct ifreq ifr;
    strcpy(ifr.ifr_name, "vcan0");
    ioctl(sock, SIOCGIFINDEX, &ifr);

    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(sock, (struct sockaddr *)&addr, sizeof(addr));

    struct can_frame frame;
    printf("Listening on vcan0...\n");

    while (1) {
        int nbytes = read(sock, &frame, sizeof(frame));
        if (nbytes < 0) {
            perror("Read error");
            break;
        }

        printf("Received: ID=0x%X  DLC=%d  Data=",
               frame.can_id, frame.can_dlc);

        for (int i = 0; i < frame.can_dlc; i++) {
            printf("%02X ", frame.data[i]);
        }
        printf("\n");
    }

    close(sock);
    return 0;
}
Paste with Ctrl+Shift+V

Step 4 — Save and exit

Ctrl+O → Enter → Ctrl+X


Step 5 — Compile
bashgcc can_recv.c -o can_recv


PART 3 — Run Both Together
Now you have two Ubuntu windows open. Here's what to do in each:

Window 2 — start the receiver first:
bash./can_recv
You should see:
Listening on vcan0...
It just waits. Good.

Window 1 — send a frame:
bash./can_send
Window 1 prints:
CAN frame sent! ID: 0x100  Data: DE AD BE EF
Window 2 immediately prints:
Received: ID=0x100  DLC=4  Data=DE AD BE EF

Try sending multiple times:
bash./can_send ; ./can_send ; ./can_send
Window 2 should print 3 lines.

Try mixing with cansend:
bashcansend vcan0 200#AABBCCDD
Window 2 catches that too:
Received: ID=0x200  DLC=4  Data=AA BB CC DD

To stop the receiver when done:
Press Ctrl+C in Window 2.

Try all of this and paste what both windows show. Once it works we move to Lesson 4 — filtering by CAN ID.when i am trying to save its showing me write to file: can_send.cThat's completely normal! That's nano asking you to confirm the filename.
Just press Enter.
That's it. Then press Ctrl+X to exit.gcc can_send.c -o can_send
can_send.c: In function ‘main’:
can_send.c:63:1: error: expected declaration or statement at end of input
   63 | }
      | ^
can_send.c:63:1: error: expected declaration or statement at end of inputThis means there's an extra } at the end or the code got pasted with some duplication. Let's fix it.
Run:
bashcat -n can_send.c
Paste the full output here and I'll tell you exactly what to fix.yes, i made a mistake , how to open the codeJust run:
bashnano can_send.c
It opens the existing file with your code already in it.
Then use:

Arrow keys to move around
Ctrl+K to delete an entire line
Ctrl+O → Enter to save
Ctrl+X to exit

Paste what you see in the file and I'll tell you exactly which line to fix.all commands are working
go for lesson 4Lesson 4 — CAN Filters
The Problem Without Filters
Right now your can_recv.c receives every single frame on the bus — all IDs, all data. In a real car or industrial system there could be hundreds of different message IDs flying around constantly.
A brake controller doesn't care about infotainment messages. An engine ECU doesn't care about window motor status. Each node should only process what's relevant to it.
That's what filters do.

How CAN Filtering Works in SocketCAN
SocketCAN filters work on two values:
cstruct can_filter {
    canid_t can_id;
    canid_t can_mask;
};
The rule is simple:
received_frame_id & can_mask == can_id & can_mask → accept
otherwise → ignore
The mask decides which bits of the ID to check:

0xFFF → check all 11 bits → exact match only
0xFF0 → check upper 8 bits → accept a range of IDs
0x000 → check no bits → accept everything


Three Filter Examples
Scenariocan_idcan_maskAcceptsExact match0x1000xFFFOnly 0x100Range0x1000xFF00x100 to 0x10FAccept all0x0000x000Everything

Updated can_recv.c — With Filter
Open the file:
bashnano can_recv.c
Delete everything with Alt+T (deletes all text), then paste this:
c#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>

int main() {
    // 1. Create socket and bind (same as before)
    int sock = socket(AF_CAN, SOCK_RAW, CAN_RAW);

    struct ifreq ifr;
    strcpy(ifr.ifr_name, "vcan0");
    ioctl(sock, SIOCGIFINDEX, &ifr);

    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(sock, (struct sockaddr *)&addr, sizeof(addr));

    // 2. Set up a filter — only accept ID 0x100
    struct can_filter filter[1];
    filter[0].can_id   = 0x100;
    filter[0].can_mask = 0xFFF;

    setsockopt(sock, SOL_CAN_RAW, CAN_RAW_FILTER,
               &filter, sizeof(filter));

    // 3. Receive loop
    struct can_frame frame;
    printf("Listening for ID 0x100 only...\n");

    while (1) {
        int nbytes = read(sock, &frame, sizeof(frame));
        if (nbytes < 0) {
            perror("Read error");
            break;
        }

        printf("Received: ID=0x%X  DLC=%d  Data=",
               frame.can_id, frame.can_dlc);

        for (int i = 0; i < frame.can_dlc; i++) {
            printf("%02X ", frame.data[i]);
        }
        printf("\n");
    }

    close(sock);
    return 0;
}
Save and exit: Ctrl+O → Enter → Ctrl+X
Compile:
bashgcc can_recv.c -o can_recv

Test the Filter
Window 2 — start filtered receiver:
bash./can_recv
Output:
Listening for ID 0x100 only...
Window 1 — send different IDs:
bashcansend vcan0 100#DEADBEEF
cansend vcan0 200#AABBCCDD
cansend vcan0 300#11223344
cansend vcan0 100#CAFEBABE
Window 2 should show ONLY:
Received: ID=0x100  DLC=4  Data=DE AD BE EF
Received: ID=0x100  DLC=4  Data=CA FE BA BE
Frames with ID 0x200 and 0x300 are completely ignored — the kernel drops them before they even reach your program.

The New Section Explained
cstruct can_filter filter[1];
filter[0].can_id   = 0x100;
filter[0].can_mask = 0xFFF;

setsockopt(sock, SOL_CAN_RAW, CAN_RAW_FILTER,
           &filter, sizeof(filter));

struct can_filter filter[1] — array of filters, we have one filter here
filter[0].can_id = 0x100 — the ID we want
filter[0].can_mask = 0xFFF — check all 11 bits → exact match
setsockopt(...) — applies the filter to the socket

setsockopt is a standard Linux socket call for configuring socket behavior — not specific to CAN, you'll see it in TCP/IP programming too.

Multiple Filters
You can filter multiple IDs at once — just expand the array:
cstruct can_filter filter[2];

filter[0].can_id   = 0x100;
filter[0].can_mask = 0xFFF;   // accept 0x100

filter[1].can_id   = 0x200;
filter[1].can_mask = 0xFFF;   // accept 0x200

setsockopt(sock, SOL_CAN_RAW, CAN_RAW_FILTER,
           &filter, sizeof(filter));
Now it accepts 0x100 and 0x200, ignores everything else.

Real World Example
In a car:
ECU (Engine Control Unit)
├── filter[0]: 0x100  ← throttle position sensor
├── filter[1]: 0x101  ← RPM sensor
└── filter[2]: 0x102  ← coolant temperature
    ignores 0x300 (brake pressure) — not relevant
    ignores 0x400 (window motor)   — not relevant
Each ECU only wakes up for messages it cares about. This keeps the system efficient and deterministic.

Try it and paste what both windows show. Once confirmed, Lesson 5 is the mini project — a proper CAN data logger in C that saves frames to a file with timestamps, like a real automotive diagnostic tool.i understand this part filter[0].can_id = 0x100; but i did not understand how mask is working here filter[0].can_mask = 0xFFF;Great question. Let me explain it visually.

How the Mask Works
The mask decides which bits of the ID to compare.
Think of it like this:

Mask bit = 1 → check this bit
Mask bit = 0 → ignore this bit


The Rule
received_id & mask == can_id & mask → accept frame

Your Case — Exact Match (mask = 0xFFF)
0xFFF in binary = 1111 1111 1111 — all 11 bits are 1, meaning check every bit.
Incoming frame ID:  0x100 = 0001 0000 0000
Mask:               0xFFF = 1111 1111 1111
                            ─────────────
AND result:                 0001 0000 0000  =  0x100

Your filter can_id:  0x100 = 0001 0000 0000
Mask:                0xFFF = 1111 1111 1111
                             ─────────────
AND result:                  0001 0000 0000  =  0x100

0x100 == 0x100 → ✅ ACCEPT
Now try with a different ID:
Incoming frame ID:  0x200 = 0010 0000 0000
Mask:               0xFFF = 1111 1111 1111
                            ─────────────
AND result:                 0010 0000 0000  =  0x200

Your filter can_id:  0x100 → AND result = 0x100

0x200 != 0x100 → ❌ REJECT

Range Match Example (mask = 0xFF0)
0xFF0 in binary = 1111 1111 0000 — last 4 bits are 0, meaning ignore the last 4 bits.
So it accepts any ID from 0x100 to 0x10F:
Incoming 0x100:  0001 0000 0000
                 AND 1111 1111 0000
                 ─────────────────
                 0001 0000 0000  =  0x100 ✅

Incoming 0x105:  0001 0000 0101
                 AND 1111 1111 0000
                 ─────────────────
                 0001 0000 0000  =  0x100 ✅

Incoming 0x200:  0010 0000 0000
                 AND 1111 1111 0000
                 ─────────────────
                 0010 0000 0000  =  0x200 ❌

Simple Summary
MaskMeaning0xFFFAll bits checked → exact ID match only0xFF0Last 4 bits ignored → accepts a range0x000No bits checked → accepts everything

Think of the mask like a stencil:

Where the stencil has a hole (bit = 0) → don't look at that bit
Where the stencil is solid (bit = 1) → check that bit

Does that make sense? Try explaining it back to me in one sentence and I'll confirm if you got it right.okay, i understand id and mask part. but i have one more question, how to put different filter for different unit. lets say for ecu i need rpm sensor but dont need tire pressure. but for different case i need tire pressure. how to define thisGreat practical question. Each separate program (process) has its own socket and its own filters — they don't affect each other.

Real World Setup
CAN Bus (vcan0)
├── ecu_program    → filter: 0x101 (RPM only)
├── abs_program    → filter: 0x300 (tire pressure only)
└── logger_program → no filter (logs everything)
All three run at the same time, all connected to the same bus, each with different filters.

How to Define It in Code
ECU Program — wants RPM, ignores tire pressure
cstruct can_filter filter[1];
filter[0].can_id   = 0x101;   // RPM sensor ID
filter[0].can_mask = 0xFFF;   // exact match only

setsockopt(sock, SOL_CAN_RAW, CAN_RAW_FILTER,
           &filter, sizeof(filter));

ABS Program — wants tire pressure, ignores RPM
cstruct can_filter filter[1];
filter[0].can_id   = 0x300;   // tire pressure ID
filter[0].can_mask = 0xFFF;

setsockopt(sock, SOL_CAN_RAW, CAN_RAW_FILTER,
           &filter, sizeof(filter));

Dashboard Program — wants both RPM and tire pressure
cstruct can_filter filter[2];

filter[0].can_id   = 0x101;   // RPM
filter[0].can_mask = 0xFFF;

filter[1].can_id   = 0x300;   // tire pressure
filter[1].can_mask = 0xFFF;

setsockopt(sock, SOL_CAN_RAW, CAN_RAW_FILTER,
           &filter, sizeof(filter));

Key Point
The filter lives inside the socket, not on the bus. So:
CAN Bus sends:  0x101, 0x200, 0x300, 0x400
                          │
              ┌───────────┼───────────┐
              ▼           ▼           ▼
         ecu_program  abs_program  dashboard
         sock+filter  sock+filter  sock+filter
           0x101        0x300      0x101+0x300
              │           │           │
           sees         sees        sees
           0x101        0x300     0x101+0x300
           only         only        only
Every program gets all frames delivered to its socket first, then the kernel applies that socket's filter and drops what doesn't match — before your code even sees it.

How to Test This on vcan0
You can simulate this right now with two terminal windows running different receiver programs simultaneously.
Create two files:
ecu_recv.c — RPM only (ID 0x101):
cfilter[0].can_id   = 0x101;
filter[0].can_mask = 0xFFF;
abs_recv.c — tire pressure only (ID 0x300):
cfilter[0].can_id   = 0x300;
filter[0].can_mask = 0xFFF;
Then run both at the same time:
Window 1: ./ecu_recv
Window 2: ./abs_recv
Window 3: cansend vcan0 101#1234 ; cansend vcan0 300#5678
Window 1 only prints 0x101, Window 2 only prints 0x300 — even though both are on the same bus.

Want to try building those two programs as a practical exercise before moving to Lesson 5?yes pleaseExercise — Two Programs, Same Bus, Different Filters

PART 1 — Create ecu_recv.c
Open the file:
bashnano ecu_recv.c
Paste this:
c#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>

int main() {
    int sock = socket(AF_CAN, SOCK_RAW, CAN_RAW);

    struct ifreq ifr;
    strcpy(ifr.ifr_name, "vcan0");
    ioctl(sock, SIOCGIFINDEX, &ifr);

    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(sock, (struct sockaddr *)&addr, sizeof(addr));

    // ECU filter — RPM sensor only (ID 0x101)
    struct can_filter filter[1];
    filter[0].can_id   = 0x101;
    filter[0].can_mask = 0xFFF;
    setsockopt(sock, SOL_CAN_RAW, CAN_RAW_FILTER,
               &filter, sizeof(filter));

    struct can_frame frame;
    printf("ECU listening for RPM (0x101) only...\n");

    while (1) {
        int nbytes = read(sock, &frame, sizeof(frame));
        if (nbytes < 0) {
            perror("Read error");
            break;
        }
        printf("[ECU]  RPM frame received: ID=0x%X  Data=",
               frame.can_id);
        for (int i = 0; i < frame.can_dlc; i++) {
            printf("%02X ", frame.data[i]);
        }
        printf("\n");
    }

    close(sock);
    return 0;
}
Save and exit: Ctrl+O → Enter → Ctrl+X
Compile:
bashgcc ecu_recv.c -o ecu_recv

PART 2 — Create abs_recv.c
Open the file:
bashnano abs_recv.c
Paste this:
c#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>

int main() {
    int sock = socket(AF_CAN, SOCK_RAW, CAN_RAW);

    struct ifreq ifr;
    strcpy(ifr.ifr_name, "vcan0");
    ioctl(sock, SIOCGIFINDEX, &ifr);

    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(sock, (struct sockaddr *)&addr, sizeof(addr));

    // ABS filter — tire pressure only (ID 0x300)
    struct can_filter filter[1];
    filter[0].can_id   = 0x300;
    filter[0].can_mask = 0xFFF;
    setsockopt(sock, SOL_CAN_RAW, CAN_RAW_FILTER,
               &filter, sizeof(filter));

    struct can_frame frame;
    printf("ABS listening for tire pressure (0x300) only...\n");

    while (1) {
        int nbytes = read(sock, &frame, sizeof(frame));
        if (nbytes < 0) {
            perror("Read error");
            break;
        }
        printf("[ABS]  Tire pressure frame received: ID=0x%X  Data=",
               frame.can_id);
        for (int i = 0; i < frame.can_dlc; i++) {
            printf("%02X ", frame.data[i]);
        }
        printf("\n");
    }

    close(sock);
    return 0;
}
Save and exit: Ctrl+O → Enter → Ctrl+X
Compile:
bashgcc abs_recv.c -o abs_recv

PART 3 — Run Everything Together
You need 3 Ubuntu windows open at the same time.
Open two more Ubuntu windows:

Windows key → type Ubuntu → Enter (do this twice)


Window 1 — start ECU receiver:
bash./ecu_recv
Output:
ECU listening for RPM (0x101) only...

Window 2 — start ABS receiver:
bash./abs_recv
Output:
ABS listening for tire pressure (0x300) only...

Window 3 — send all frame types:
bashcansend vcan0 101#1111 ; cansend vcan0 300#2222 ; cansend vcan0 200#3333 ; cansend vcan0 101#4444 ; cansend vcan0 300#5555

What You Should See
Window 1 (ECU) — only sees 0x101:
ECU listening for RPM (0x101) only...
[ECU]  RPM frame received: ID=0x101  Data=11 11
[ECU]  RPM frame received: ID=0x101  Data=44 44
Window 2 (ABS) — only sees 0x300:
ABS listening for tire pressure (0x300) only...
[ABS]  Tire pressure frame received: ID=0x300  Data=22 22
[ABS]  Tire pressure frame received: ID=0x300  Data=55 55
Frame 0x200 is completely ignored by both — nobody cares about it.

What This Demonstrates
Bus receives:  0x101  0x300  0x200  0x101  0x300
                 │      │      │      │      │
            ┌────┘   ┌──┘      │   ┌──┘   ┌─┘
            ▼        ▼         ▼   ▼      ▼
         ecu_recv  abs_recv   DROPPED (no listener)
         prints    prints
         0x101     0x300
         only      only

Try it and paste what all three windows show. Once confirmed we move to Lesson 5 — the mini project, a full CAN data logger that saves frames to a file with timestamps.go to L5Lesson 5 — CAN Data Logger (Mini Project)
This is your first complete, real-world style embedded Linux tool. A data logger is used in automotive and industrial work to record everything happening on a CAN bus — for debugging, testing, and analysis.

What It Will Do

Connect to vcan0
Receive all CAN frames (no filter — logs everything)
Print each frame to terminal with a timestamp
Save every frame to a log file (can_log.txt)
Run until you press Ctrl+C
Print a summary at the end — how many frames were logged


The New Concepts in This Lesson
ConceptWhat it isgettimeofday()Get current time in microsecondssignal()Catch Ctrl+C and handle it gracefullyfprintf()Write to a file instead of terminalfopen/fclose()Open and close a log file

Create the Logger
bashnano can_logger.c
Paste this:
c#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>

// ─── Global variables ───────────────────────────────
int sock;
FILE *logfile;
int frame_count = 0;
int running = 1;

// ─── Signal handler — runs when Ctrl+C is pressed ───
void handle_exit(int sig) {
    running = 0;
}

// ─── Get timestamp in seconds since program started ─
double get_timestamp(struct timeval *start) {
    struct timeval now;
    gettimeofday(&now, NULL);
    double seconds = (now.tv_sec - start->tv_sec)
                   + (now.tv_usec - start->tv_usec) / 1000000.0;
    return seconds;
}

// ─── Main ────────────────────────────────────────────
int main() {
    // 1. Set up Ctrl+C handler
    signal(SIGINT, handle_exit);

    // 2. Open log file
    logfile = fopen("can_log.txt", "w");
    if (logfile == NULL) {
        perror("Failed to open log file");
        return 1;
    }
    printf("Logging to can_log.txt\n");
    fprintf(logfile, "timestamp(s)   CAN_ID   DLC   Data\n");
    fprintf(logfile, "─────────────────────────────────────────\n");

    // 3. Create socket and bind
    sock = socket(AF_CAN, SOCK_RAW, CAN_RAW);
    if (sock < 0) {
        perror("Socket error");
        return 1;
    }

    struct ifreq ifr;
    strcpy(ifr.ifr_name, "vcan0");
    ioctl(sock, SIOCGIFINDEX, &ifr);

    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family    = AF_CAN;
    addr.can_ifindex   = ifr.ifr_ifindex;
    bind(sock, (struct sockaddr *)&addr, sizeof(addr));

    // 4. Record start time
    struct timeval start;
    gettimeofday(&start, NULL);

    printf("CAN Logger started. Press Ctrl+C to stop.\n");
    printf("─────────────────────────────────────────\n");
    printf("%-14s %-8s %-5s %s\n",
           "Time(s)", "ID", "DLC", "Data");
    printf("─────────────────────────────────────────\n");

    // 5. Main receive loop
    struct can_frame frame;

    while (running) {
        int nbytes = read(sock, &frame, sizeof(frame));
        if (nbytes < 0) break;

        double timestamp = get_timestamp(&start);
        frame_count++;

        // Print to terminal
        printf("%-14.4f 0x%-6X %-5d ",
               timestamp, frame.can_id, frame.can_dlc);
        for (int i = 0; i < frame.can_dlc; i++) {
            printf("%02X ", frame.data[i]);
        }
        printf("\n");

        // Write to log file
        fprintf(logfile, "%-14.4f 0x%-6X %-5d ",
                timestamp, frame.can_id, frame.can_dlc);
        for (int i = 0; i < frame.can_dlc; i++) {
            fprintf(logfile, "%02X ", frame.data[i]);
        }
        fprintf(logfile, "\n");
    }

    // 6. Summary
    printf("─────────────────────────────────────────\n");
    printf("Logger stopped. Total frames: %d\n", frame_count);
    fprintf(logfile, "─────────────────────────────────────────\n");
    fprintf(logfile, "Total frames logged: %d\n", frame_count);

    // 7. Clean up
    fclose(logfile);
    close(sock);
    return 0;
}
Save and exit: Ctrl+O → Enter → Ctrl+X
Compile:
bashgcc can_logger.c -o can_logger

Run the Logger
Window 1 — start the logger:
bash./can_logger
You should see:
Logging to can_log.txt
CAN Logger started. Press Ctrl+C to stop.
─────────────────────────────────────────
Time(s)        ID       DLC   Data
─────────────────────────────────────────

Window 2 — send a mix of frames:
bashcansend vcan0 101#1111 ; cansend vcan0 300#AABBCCDD ; cansend vcan0 200#FF ; cansend vcan0 100#DEADBEEF ; cansend vcan0 101#2222
Window 1 should show:
Time(s)        ID       DLC   Data
─────────────────────────────────────────
0.0023         0x101    2     11 11
0.0031         0x300    4     AA BB CC DD
0.0038         0x200    1     FF
0.0045         0x100    4     DE AD BE EF
0.0051         0x101    2     22 22

Press Ctrl+C to stop:
─────────────────────────────────────────
Logger stopped. Total frames: 5

Check the Log File
bashcat can_log.txt
You should see the exact same data saved to disk:
timestamp(s)   CAN_ID   DLC   Data
─────────────────────────────────────────
0.0023         0x101    2     11 11
0.0031         0x300    4     AA BB CC DD
0.0038         0x200    1     FF
0.0045         0x100    4     DE AD BE EF
0.0051         0x101    2     22 22
─────────────────────────────────────────
Total frames logged: 5

Code Walkthrough — New Parts Only

Signal Handler
cvoid handle_exit(int sig) {
    running = 0;
}
signal(SIGINT, handle_exit);
SIGINT is the signal sent when you press Ctrl+C. Instead of killing the program instantly, we catch it and set running = 0 — so the while loop exits cleanly, summary prints, and files close properly.
Without this, Ctrl+C kills the program instantly and the log file might not save correctly.

Timestamp
cdouble get_timestamp(struct timeval *start) {
    struct timeval now;
    gettimeofday(&now, NULL);
    double seconds = (now.tv_sec - start->tv_sec)
                   + (now.tv_usec - start->tv_usec) / 1000000.0;
    return seconds;
}
gettimeofday() gives you current time in seconds + microseconds. We subtract the start time to get elapsed time since logging began — so the first frame always shows ~0.000 seconds.

File Writing
clogfile = fopen("can_log.txt", "w");   // open file for writing
fprintf(logfile, "...", ...);           // write to file (same as printf)
fclose(logfile);                        // close when done
fprintf works exactly like printf — same format strings, same arguments — but writes to a file instead of the terminal.

What You've Built
vcan0 bus
   │
   ▼
can_logger
   ├── prints to terminal (printf)     ← you see it live
   └── writes to can_log.txt           ← saved for later analysis
        ├── timestamp
        ├── CAN ID
        ├── DLC
        └── data bytes
This is genuinely what automotive engineers use — candump -l does something very similar, but now you built it yourself from scratch.

Lesson Summary — What You've Covered in Phase 3
LessonTopicStatus1CAN Bus fundamentals✅2SocketCAN + vcan0 + candump/cansend✅3C sender and receiver programs✅4Filtering by CAN ID✅5CAN data logger with timestamps✅

What's Next — Phase 3 Extension
You've completed the core CAN Bus lessons. The natural next step before moving to C# is one more topic:
Lesson 6 — Periodic Sender — simulating a real sensor that broadcasts data at a fixed rate (like an RPM sensor sending every 10ms). This is how real ECUs behave and sets you up perfectly for the C# transition.
Want to go for Lesson 6, or do you want to try the logger first and come back?i did not learn or get any info about canh and canl, or differentialGood catch — I mentioned CAN_H and CAN_L briefly in Lesson 1 but never went deep on it. Let me explain it properly now.

CAN_H and CAN_L — The Physical Layer

The Two Wires
Every CAN Bus has exactly two wires:

CAN_H — CAN High
CAN_L — CAN Low

They always carry opposite signals — when one goes up, the other goes down. This is called a differential pair.

Why Two Wires Instead of One?
Most simple protocols (UART, basic sensors) use a single wire — signal voltage vs ground. Like this:
Single wire:
Wire ──── 5V = logic 1
Wire ──── 0V = logic 0
Ground ─────────────────
The problem — in a car engine bay or factory floor, there's massive electrical noise from motors, ignition systems, and power lines. This noise adds voltage to your signal wire and corrupts the data.

How Differential Signaling Solves This
CAN uses two wires carrying opposite signals:
CAN_H ──── 3.5V ────
CAN_L ──── 1.5V ────
Difference = 3.5 - 1.5 = 2.0V = logic 1 (dominant)

CAN_H ──── 2.5V ────
CAN_L ──── 2.5V ────
Difference = 2.5 - 2.5 = 0.0V = logic 0 (recessive)
The receiver doesn't look at the voltage of either wire alone — it looks at the difference between them.

Why Noise Doesn't Matter
When electrical noise hits the cable, it hits both wires equally — because they run side by side (often twisted together):
Without noise:
CAN_H = 3.5V,  CAN_L = 1.5V,  difference = 2.0V ✅

Noise adds 1.0V to both wires:
CAN_H = 4.5V,  CAN_L = 2.5V,  difference = 2.0V ✅

Noise adds 2.0V to both wires:
CAN_H = 5.5V,  CAN_L = 3.5V,  difference = 2.0V ✅
The difference stays the same no matter how much noise hits the cable. That's the genius of differential signaling — noise cancels itself out.

Dominant vs Recessive — The Two States
CAN only has two logical states:
StateCAN_HCAN_LDifferenceMeaningDominant3.5V1.5V2.0VLogic 0Recessive2.5V2.5V0.0VLogic 1
This naming feels backwards at first — dominant = 0, recessive = 1. The reason is arbitration — a dominant bit (0) always wins over a recessive bit (1) when two nodes transmit simultaneously. Lower ID wins because it has more dominant (0) bits early on.

Twisted Pair Cable
In real CAN installations the two wires are twisted around each other:
CAN_H ─╮ ╭─╮ ╭─╮ ╭─
        ╰─╯ ╰─╯ ╰─╯
CAN_L ─╯   ╰─╯  ╰─╮
Twisting makes both wires pick up exactly the same noise at the same time — which makes the noise cancel even more effectively when you subtract CAN_H - CAN_L.
This is why it's called twisted pair and why CAN works reliably in electrically noisy environments like cars and factories.

Termination Resistors
At each end of the CAN bus there is a 120 ohm resistor connected between CAN_H and CAN_L:
Node A          Node B          Node C
  │               │               │
CAN_H ══════════════════════════════
CAN_L ══════════════════════════════
  │                               │
 120Ω                            120Ω
Without these resistors, the electrical signal reflects back from the end of the wire like an echo — corrupting data. The resistors absorb the signal at each end and prevent reflections.
Two resistors of 120Ω in parallel = 60Ω total bus impedance, which is the standard for CAN.

How This Relates to vcan0
On your virtual vcan0 — none of this physical layer exists. There are no wires, no voltages, no noise, no resistors. It's pure software simulation of the logical layer only.
That's why vcan0 is perfect for learning the software side, but you'd need real hardware (MCP2515 module + Raspberry Pi) to see the actual electrical signals on an oscilloscope.

Quick Comparison to I3C (Your Background)
FeatureI3CCANWires2 (SDA + SCL)2 (CAN_H + CAN_L)SignalingSingle endedDifferentialNoise immunityModerateVery highCable lengthShort (PCB)Long (up to 40m)TerminationPull-up resistors120Ω at each end
I3C uses single ended signaling — voltage vs ground. That's fine on a PCB where distances are tiny and noise is low. CAN uses differential because it needs to work over long cables in noisy environments.

Summary
CAN_H and CAN_L always move in opposite directions
        │
        ▼
Receiver measures the DIFFERENCE (CAN_H - CAN_L)
        │
        ▼
Noise hits both wires equally → cancels out in subtraction
        │
        ▼
Signal remains clean even in noisy environments
Does that fully clear it up? Any part you want me to go deeper on — the voltage levels, the termination resistors, or how arbitration works at the electrical level?i can stop the window 1,  i am pressinf ctl+cThat's because read() is blocking — it's waiting for a frame and ignoring Ctrl+C while it waits.
This is a known limitation of our current code. The signal handler sets running = 0 but the program is stuck waiting at read() and never checks running again.

Quick Fix — Send One Dummy Frame to Unblock It
While the logger is still running, go to Window 2 and send any frame:
bashcansend vcan0 999#00
This unblocks the read(), the loop checks running = 0, and exits cleanly.
Then immediately press Ctrl+C in Window 1.

Proper Fix — Non-Blocking Read with Timeout
The real solution is to tell the socket to stop waiting after a short time, check the running flag, then go back to waiting. Add this after your bind() call:
c// Set read timeout to 1 second
struct timeval timeout;
timeout.tv_sec  = 1;
timeout.tv_usec = 0;
setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,
           &timeout, sizeof(timeout));
Now read() waits maximum 1 second, then returns even if no frame arrived. The loop checks running, and if Ctrl+C was pressed it exits cleanly.

Want me to give you the full updated can_logger.c with this fix included?yesbashnano can_logger.c
First delete everything inside — press Alt+T to select all, then type or paste the new code:
c#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>

// ─── Global variables ────────────────────────────────
int sock;
FILE *logfile;
int frame_count = 0;
volatile int running = 1;

// ─── Signal handler ──────────────────────────────────
void handle_exit(int sig) {
    running = 0;
}

// ─── Timestamp ───────────────────────────────────────
double get_timestamp(struct timeval *start) {
    struct timeval now;
    gettimeofday(&now, NULL);
    return (now.tv_sec  - start->tv_sec)
         + (now.tv_usec - start->tv_usec) / 1000000.0;
}

// ─── Main ────────────────────────────────────────────
int main() {
    // 1. Ctrl+C handler
    signal(SIGINT, handle_exit);

    // 2. Open log file
    logfile = fopen("can_log.txt", "w");
    if (logfile == NULL) {
        perror("Failed to open log file");
        return 1;
    }
    fprintf(logfile, "timestamp(s)   CAN_ID   DLC   Data\n");
    fprintf(logfile, "-----------------------------------------\n");

    // 3. Create socket
    sock = socket(AF_CAN, SOCK_RAW, CAN_RAW);
    if (sock < 0) {
        perror("Socket error");
        return 1;
    }

    // 4. Bind to vcan0
    struct ifreq ifr;
    strcpy(ifr.ifr_name, "vcan0");
    ioctl(sock, SIOCGIFINDEX, &ifr);

    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(sock, (struct sockaddr *)&addr, sizeof(addr));

    // 5. Set read timeout — 1 second
    //    read() returns every 1s even if no frame arrived
    //    so we can check the running flag and exit cleanly
    struct timeval timeout;
    timeout.tv_sec  = 1;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,
               &timeout, sizeof(timeout));

    // 6. Record start time
    struct timeval start;
    gettimeofday(&start, NULL);

    printf("Logging to can_log.txt\n");
    printf("CAN Logger started. Press Ctrl+C to stop.\n");
    printf("-----------------------------------------\n");
    printf("%-14s %-8s %-5s %s\n", "Time(s)", "ID", "DLC", "Data");
    printf("-----------------------------------------\n");

    // 7. Main receive loop
    struct can_frame frame;

    while (running) {
        int nbytes = read(sock, &frame, sizeof(frame));

        if (nbytes < 0) {
            // timeout expired — no frame arrived in 1 second
            // just loop back and check running flag
            continue;
        }

        double timestamp = get_timestamp(&start);
        frame_count++;

        // Print to terminal
        printf("%-14.4f 0x%-6X %-5d ",
               timestamp, frame.can_id, frame.can_dlc);
        for (int i = 0; i < frame.can_dlc; i++) {
            printf("%02X ", frame.data[i]);
        }
        printf("\n");

        // Write to log file
        fprintf(logfile, "%-14.4f 0x%-6X %-5d ",
                timestamp, frame.can_id, frame.can_dlc);
        for (int i = 0; i < frame.can_dlc; i++) {
            fprintf(logfile, "%02X ", frame.data[i]);
        }
        fprintf(logfile, "\n");
    }

    // 8. Summary
    printf("-----------------------------------------\n");
    printf("Logger stopped. Total frames: %d\n", frame_count);
    fprintf(logfile, "-----------------------------------------\n");
    fprintf(logfile, "Total frames logged: %d\n", frame_count);

    // 9. Clean up
    fclose(logfile);
    close(sock);
    return 0;
}
Save and exit: Ctrl+O → Enter → Ctrl+X
Compile:
bashgcc can_logger.c -o can_logger

What Changed
Only two things compared to the old version:
1. volatile int running = 1
The word volatile tells the compiler — this variable can change at any time from outside (the signal handler). Without it, the compiler might optimize away the check thinking "running never changes inside the loop."
2. Read timeout + continue
c// after bind():
struct timeval timeout;
timeout.tv_sec  = 1;
timeout.tv_usec = 0;
setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,
           &timeout, sizeof(timeout));

// inside the loop:
if (nbytes < 0) {
    continue;   // timeout — go back and check running
}
Now every 1 second read() wakes up automatically, the loop checks running, and if Ctrl+C was pressed it exits. If no Ctrl+C, it goes straight back to waiting.

Test It
Window 1:
bash./can_logger
Window 2 — send some frames:
bashcansend vcan0 101#1111 ; cansend vcan0 300#AABBCCDD ; cansend vcan0 100#DEADBEEF
Now press Ctrl+C in Window 1 — it should exit cleanly within 1 second and print:
-----------------------------------------
Logger stopped. Total frames: 3
Then check the log file:
bashcat can_log.txt
Let me know what you see!go for Lesson 6Lesson 6 — Periodic Sender (Simulating a Real Sensor)

What is a Periodic Sender?
In real automotive and industrial systems, sensors don't send data only when asked. They broadcast continuously at a fixed rate:
RPM sensor      → sends every 10ms  (100 times per second)
Temperature     → sends every 100ms (10 times per second)
Tire pressure   → sends every 500ms (2 times per second)
This is called periodic transmission and it's how almost every real ECU behaves. Up until now you've been sending frames manually with cansend. This lesson simulates a real sensor automatically.

New Concepts in This Lesson
ConceptWhat it isnanosleep()Pause execution for precise time intervalsstruct timespecTime structure for nanosecond precisionSimulated dataFake but realistic sensor valuesMultiple message typesDifferent IDs at different rates

Create the Periodic Sender
bashnano can_periodic.c
Paste this:
c#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>

// ─── Global ──────────────────────────────────────────
volatile int running = 1;
int sock;

// ─── Signal handler ──────────────────────────────────
void handle_exit(int sig) {
    running = 0;
}

// ─── Sleep for milliseconds ──────────────────────────
void sleep_ms(int ms) {
    struct timespec ts;
    ts.tv_sec  = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}

// ─── Send a CAN frame ────────────────────────────────
void send_frame(int can_id, uint8_t *data, int dlc) {
    struct can_frame frame;
    memset(&frame, 0, sizeof(frame));
    frame.can_id  = can_id;
    frame.can_dlc = dlc;
    memcpy(frame.data, data, dlc);
    write(sock, &frame, sizeof(frame));
}

// ─── Main ────────────────────────────────────────────
int main() {
    // 1. Ctrl+C handler
    signal(SIGINT, handle_exit);

    // 2. Create socket and bind
    sock = socket(AF_CAN, SOCK_RAW, CAN_RAW);
    if (sock < 0) {
        perror("Socket error");
        return 1;
    }

    struct ifreq ifr;
    strcpy(ifr.ifr_name, "vcan0");
    ioctl(sock, SIOCGIFINDEX, &ifr);

    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(sock, (struct sockaddr *)&addr, sizeof(addr));

    printf("Periodic CAN sender started. Press Ctrl+C to stop.\n");
    printf("Sending:\n");
    printf("  0x101 — RPM sensor        every 100ms\n");
    printf("  0x102 — Temperature       every 500ms\n");
    printf("  0x300 — Tire pressure     every 1000ms\n");
    printf("------------------------------------------\n");

    // 3. Counters to track timing
    int cycle      = 0;   // counts in 100ms steps
    int rpm        = 800; // starting RPM (idle)
    int temp       = 90;  // engine temp celsius
    int pressure   = 32;  // tire pressure PSI

    while (running) {
        // ── Every 100ms — RPM sensor (0x101) ──────────
        // RPM slowly increases from 800 to 3000 then resets
        rpm += 50;
        if (rpm > 3000) rpm = 800;

        // Pack RPM into 2 bytes (big endian)
        uint8_t rpm_data[2];
        rpm_data[0] = (rpm >> 8) & 0xFF;  // high byte
        rpm_data[1] =  rpm       & 0xFF;  // low byte
        send_frame(0x101, rpm_data, 2);
        printf("[100ms] RPM:  %4d  → 0x%02X%02X\n",
               rpm, rpm_data[0], rpm_data[1]);

        // ── Every 500ms — Temperature (0x102) ─────────
        if (cycle % 5 == 0) {
            temp += 1;
            if (temp > 105) temp = 90;

            uint8_t temp_data[1];
            temp_data[0] = (uint8_t)temp;
            send_frame(0x102, temp_data, 1);
            printf("[500ms] Temp: %3d°C → 0x%02X\n",
                   temp, temp_data[0]);
        }

        // ── Every 1000ms — Tire pressure (0x300) ──────
        if (cycle % 10 == 0) {
            pressure += 1;
            if (pressure > 36) pressure = 32;

            uint8_t psi_data[1];
            psi_data[0] = (uint8_t)pressure;
            send_frame(0x300, psi_data, 1);
            printf("[1000ms] Pressure: %2d PSI → 0x%02X\n",
                   pressure, psi_data[0]);
        }

        cycle++;
        sleep_ms(100);   // wait 100ms before next cycle
    }

    printf("------------------------------------------\n");
    printf("Sender stopped after %d cycles.\n", cycle);
    close(sock);
    return 0;
}
Save and exit: Ctrl+O → Enter → Ctrl+X
Compile:
bashgcc can_periodic.c -o can_periodic

Run It — Two Ways

Option A — Watch with candump
Window 1 — start sender:
bash./can_periodic
Window 2 — watch raw frames:
bashcandump vcan0
You'll see frames arriving at different rates — 0x101 every 100ms, 0x102 every 500ms, 0x300 every 1000ms.

Option B — Run with Logger (Best Option)
Window 1 — start the logger:
bash./can_logger
Window 2 — start the periodic sender:
bash./can_periodic
Now your logger automatically records every frame the sensor sends — exactly like a real automotive diagnostic session.
After 10 seconds press Ctrl+C on both windows, then:
bashcat can_log.txt
You'll see a timestamped record of every frame.

Code Walkthrough — New Parts

sleep_ms() — Precise Timing
cvoid sleep_ms(int ms) {
    struct timespec ts;
    ts.tv_sec  = ms / 1000;
    ts.tv_nsec = (ms % 1000) * 1000000L;
    nanosleep(&ts, NULL);
}
nanosleep() pauses the program for a precise duration:

tv_sec — whole seconds
tv_nsec — nanoseconds (1ms = 1,000,000 nanoseconds)

For 100ms: tv_sec = 0, tv_nsec = 100,000,000
We use nanosleep() instead of sleep() because sleep() only handles whole seconds — too coarse for sensor timing.

send_frame() — Reusable Helper
cvoid send_frame(int can_id, uint8_t *data, int dlc) {
    struct can_frame frame;
    memset(&frame, 0, sizeof(frame));
    frame.can_id  = can_id;
    frame.can_dlc = dlc;
    memcpy(frame.data, data, dlc);
    write(sock, &frame, sizeof(frame));
}
Instead of repeating the same frame-building code three times, we put it in a function. Pass in the ID, data bytes, and length — it handles the rest. This is how real embedded code is structured.

Multi-Rate Timing with cycle counter
cint cycle = 0;

while (running) {
    // runs every 100ms — always
    send_frame(0x101, ...);

    // runs every 500ms — every 5th cycle
    if (cycle % 5 == 0) {
        send_frame(0x102, ...);
    }

    // runs every 1000ms — every 10th cycle
    if (cycle % 10 == 0) {
        send_frame(0x300, ...);
    }

    cycle++;
    sleep_ms(100);
}
The base tick is 100ms. Everything else is a multiple of that:

500ms = every 5 ticks (cycle % 5 == 0)
1000ms = every 10 ticks (cycle % 10 == 0)

This is called a cyclic scheduler — a fundamental pattern in embedded real-time systems. Real automotive ECUs use this exact approach.

Packing RPM into 2 Bytes
crpm_data[0] = (rpm >> 8) & 0xFF;  // high byte
rpm_data[1] =  rpm       & 0xFF;  // low byte
RPM value of 1500 = 0x05DC in hex:
0x05DC >> 8      = 0x05  → high byte
0x05DC  & 0xFF   = 0xDC  → low byte
So it's sent as 05 DC over CAN. The receiver does the reverse to reconstruct the value:
crpm = (data[0] << 8) | data[1];
This is called big endian packing — most significant byte first. You'll see this constantly in CAN data encoding.

What You Should See
Window 1 (sender):
Periodic CAN sender started. Press Ctrl+C to stop.
Sending:
  0x101 — RPM sensor        every 100ms
  0x102 — Temperature       every 500ms
  0x300 — Tire pressure     every 1000ms
------------------------------------------
[100ms]  RPM:   850  → 0x0352
[100ms]  RPM:   900  → 0x0384
[100ms]  RPM:   950  → 0x03B6
[100ms]  RPM:  1000  → 0x03E8
[100ms]  RPM:  1050  → 0x041A
[500ms]  Temp:  91°C → 0x5B
[100ms]  RPM:  1100  → 0x044C
...
[1000ms] Pressure: 33 PSI → 0x21
Window 2 (candump):
vcan0  101  [2]  03 52
vcan0  101  [2]  03 84
vcan0  101  [2]  03 B6
vcan0  101  [2]  03 E8
vcan0  101  [2]  04 1A
vcan0  102  [1]  5B
vcan0  101  [2]  04 4C
vcan0  300  [1]  21

Lesson Summary — Full Phase 3 Complete
LessonTopicStatus1CAN Bus fundamentals✅2SocketCAN + vcan0 + candump/cansend✅3C sender and receiver programs✅4Filtering by CAN ID✅5CAN data logger with timestamps✅6Periodic sender — simulating real sensors✅

You Now Have a Complete Mini System
can_periodic   →  vcan0  →  can_logger
(sensor node)    (CAN bus)  (diagnostic tool)
sends RPM,                  records everything
temp, pressure              with timestamps
at real rates               to can_log.txt
This is a genuinely employable demo. If someone asked you in an interview "have you worked with CAN Bus?" — you can say yes and explain exactly what you built.

What's Next
Two directions from here:
Option A — Move to Phase 1 (C# basics) so we can eventually rewrite this logger in C#/.NET — which is the actual job stack.
Option B — One more CAN topic: DBC files — industry standard format for defining what each CAN ID means and how to decode the data bytes into real engineering values (RPM, temperature, etc).
Which do you want to go for?first d´star opt B then when finish this one, give me opt AGreat choice — DBC files are used in every real automotive project. This completes your CAN knowledge before we move to C#.

Lesson 7 — DBC Files (Decoding Real CAN Data)

The Problem Without DBC
Right now when you see this in candump:
vcan0  101  [2]  03 E8
You know it's ID 0x101 with 2 bytes of data 03 E8. But what does 03 E8 actually mean? Is it RPM? Temperature? Speed?
Without extra information — you have no idea. Raw CAN data is just bytes.

What is a DBC File?
DBC stands for DataBase CAN. It's a text file that defines:

What each CAN ID represents
How to decode the raw bytes into real values
Units, ranges, and signal names

Every automotive company (Bosch, BMW, Continental, Volkswagen) uses DBC files. Tools like CANalyzer, MATLAB, and Python's python-can all read DBC files.
Think of it as a dictionary that translates raw bytes into human readable values.

DBC File Structure
A basic DBC file looks like this:
VERSION ""

NS_ :

BS_:

BU_:

BO_ 257 RPM_Message: 2 Vector__XXX
 SG_ EngineRPM : 0|16@1+ (1,0) [0|8000] "RPM" Vector__XXX

BO_ 258 Temp_Message: 1 Vector__XXX
 SG_ EngineTemp : 0|8@1+ (1,0) [0|255] "degC" Vector__XXX

BO_ 768 TirePressure_Message: 1 Vector__XXX
 SG_ FrontLeftPSI : 0|8@1+ (1,0) [0|100] "PSI" Vector__XXX

Breaking Down the Syntax
Message Definition
BO_ 257 RPM_Message: 2 Vector__XXX
PartMeaningBO_Keyword meaning "message"257CAN ID in decimal (257 = 0x101)RPM_MessageHuman readable name2Data length in bytes (DLC)Vector__XXXSender node name (we use generic)

Signal Definition
SG_ EngineRPM : 0|16@1+ (1,0) [0|8000] "RPM" Vector__XXX
PartMeaningSG_Keyword meaning "signal"EngineRPMSignal name0Start bit position in the frame16Signal length in bits@1Byte order: 1 = little endian, 0 = big endian+Value type: + = unsigned, - = signed(1,0)Factor and offset: value = raw * factor + offset[0|8000]Min and max value"RPM"Unit

Create Your DBC File
bashnano car_signals.dbc
Paste this:
VERSION ""

NS_ :

BS_:

BU_:

BO_ 257 RPM_Message: 2 Vector__XXX
 SG_ EngineRPM : 0|16@1+ (1,0) [0|8000] "RPM" Vector__XXX

BO_ 258 Temp_Message: 1 Vector__XXX
 SG_ EngineTemp : 0|8@1+ (1,0) [0|255] "degC" Vector__XXX

BO_ 768 TirePressure_Message: 1 Vector__XXX
 SG_ FrontLeftPSI : 0|8@1+ (1,0) [0|100] "PSI" Vector__XXX
Save and exit: Ctrl+O → Enter → Ctrl+X

DBC Decoder in C
Now let's write a program that reads CAN frames and uses the DBC information to decode them into real values.
bashnano can_decoder.c
Paste this:
c#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <net/if.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <linux/can.h>
#include <linux/can/raw.h>

// ─── Signal definitions (from DBC file) ─────────────
// This is a simplified hardcoded version of what a
// real DBC parser would load dynamically from the file

typedef struct {
    int     can_id;        // CAN ID (decimal)
    char    msg_name[32];  // message name
    char    sig_name[32];  // signal name
    char    unit[16];      // unit string
    int     start_bit;     // start bit in frame
    int     length_bits;   // signal length in bits
    double  factor;        // scaling factor
    double  offset;        // offset
} DBCSignal;

// Our DBC signals — matches car_signals.dbc
DBCSignal dbc[] = {
    {257, "RPM_Message",      "EngineRPM",    "RPM",  0, 16, 1.0, 0.0},
    {258, "Temp_Message",     "EngineTemp",   "degC", 0,  8, 1.0, 0.0},
    {768, "TirePressure_Msg", "FrontLeftPSI", "PSI",  0,  8, 1.0, 0.0},
};
int dbc_count = 3;

// ─── Decode raw bytes into a value ───────────────────
double decode_signal(struct can_frame *frame, DBCSignal *sig) {
    // Extract raw value from data bytes
    // Simple version: handles signals up to 16 bits
    unsigned int raw = 0;

    if (sig->length_bits <= 8) {
        raw = frame->data[0];
    } else {
        // 16-bit little endian
        raw = frame->data[0] | (frame->data[1] << 8);
    }

    // Apply factor and offset from DBC
    // physical_value = raw * factor + offset
    return (raw * sig->factor) + sig->offset;
}

// ─── Find matching DBC signal for a CAN ID ───────────
DBCSignal* find_signal(int can_id) {
    for (int i = 0; i < dbc_count; i++) {
        if (dbc[i].can_id == can_id) {
            return &dbc[i];
        }
    }
    return NULL;
}

// ─── Global ──────────────────────────────────────────
volatile int running = 1;
void handle_exit(int sig) { running = 0; }

// ─── Main ────────────────────────────────────────────
int main() {
    signal(SIGINT, handle_exit);

    // Create socket and bind
    int sock = socket(AF_CAN, SOCK_RAW, CAN_RAW);
    if (sock < 0) {
        perror("Socket error");
        return 1;
    }

    struct ifreq ifr;
    strcpy(ifr.ifr_name, "vcan0");
    ioctl(sock, SIOCGIFINDEX, &ifr);

    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family  = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    bind(sock, (struct sockaddr *)&addr, sizeof(addr));

    // Read timeout so Ctrl+C works
    struct timeval timeout;
    timeout.tv_sec  = 1;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,
               &timeout, sizeof(timeout));

    printf("CAN Decoder running. Press Ctrl+C to stop.\n");
    printf("Using signals from car_signals.dbc\n");
    printf("------------------------------------------\n");
    printf("%-20s %-16s %-10s %s\n",
           "Message", "Signal", "Value", "Unit");
    printf("------------------------------------------\n");

    struct can_frame frame;

    while (running) {
        int nbytes = read(sock, &frame, sizeof(frame));
        if (nbytes < 0) continue;

        // Look up this CAN ID in our DBC table
        DBCSignal *sig = find_signal(frame.can_id);

        if (sig != NULL) {
            // Known signal — decode and print
            double value = decode_signal(&frame, sig);
            printf("%-20s %-16s %-10.1f %s\n",
                   sig->msg_name,
                   sig->sig_name,
                   value,
                   sig->unit);
        } else {
            // Unknown ID — print raw bytes
            printf("Unknown ID=0x%X  Raw: ", frame.can_id);
            for (int i = 0; i < frame.can_dlc; i++) {
                printf("%02X ", frame.data[i]);
            }
            printf("\n");
        }
    }

    printf("------------------------------------------\n");
    printf("Decoder stopped.\n");
    close(sock);
    return 0;
}
Save and exit: Ctrl+O → Enter → Ctrl+X
Compile:
bashgcc can_decoder.c -o can_decoder

Run Everything Together
You need 3 windows:
Window 1 — start decoder:
bash./can_decoder
Output:
CAN Decoder running. Press Ctrl+C to stop.
Using signals from car_signals.dbc
------------------------------------------
Message              Signal           Value      Unit
------------------------------------------
Window 2 — start periodic sender:
bash./can_periodic
Window 1 should now show decoded values:
Message              Signal           Value      Unit
------------------------------------------
RPM_Message          EngineRPM        850.0      RPM
RPM_Message          EngineRPM        900.0      RPM
RPM_Message          EngineRPM        950.0      RPM
Temp_Message         EngineTemp       91.0       degC
RPM_Message          EngineRPM        1000.0     RPM
TirePressure_Msg     FrontLeftPSI     33.0       PSI
Raw bytes are now human readable engineering values.

The decode_signal() Function Explained
cdouble decode_signal(struct can_frame *frame, DBCSignal *sig) {
    unsigned int raw = 0;

    if (sig->length_bits <= 8) {
        raw = frame->data[0];          // 1 byte signal
    } else {
        raw = frame->data[0]           // 2 byte signal
            | (frame->data[1] << 8);   // little endian
    }

    return (raw * sig->factor) + sig->offset;
}
This is the heart of any DBC decoder:

Extract raw bits from the frame
Apply physical = raw * factor + offset

For example with RPM = 1000:
raw bytes:    0x03 0xE8  (little endian)
raw value:    0x03E8 = 1000
factor:       1.0
offset:       0.0
result:       1000 * 1.0 + 0.0 = 1000 RPM ✅
Factor and offset become powerful when signals need scaling. For example a real temperature sensor might send values 0–255 representing -40°C to +215°C:
factor = 1.0,  offset = -40.0
raw = 130  →  130 * 1.0 + (-40) = 90°C

What You've Built — Full System
can_periodic.c          vcan0 bus         can_decoder.c
──────────────          ─────────         ─────────────
Sends every 100ms  →   carries raw   →   looks up DBC
ID 0x101 RPM           CAN frames        decodes bytes
ID 0x102 Temp                            prints values
ID 0x300 Pressure                        in engineering
                                         units
This is exactly how a real automotive diagnostic tool works — a CAN interface receives raw frames, a DBC file provides the decoding rules, and the tool shows you RPM, temperature, pressure in real units.

Lesson Summary — Phase 3 Fully Complete
LessonTopicStatus1CAN Bus fundamentals✅2SocketCAN + vcan0 + candump/cansend✅3C sender and receiver✅4CAN filtering✅5CAN data logger✅6Periodic sender✅7DBC files and signal decoding✅

What You Can Now Do

Explain CAN Bus protocol to an interviewer
Set up SocketCAN on Linux from scratch
Write C programs that send, receive, filter, and log CAN frames
Simulate real sensor nodes with periodic transmission
Decode raw CAN bytes into engineering values using DBC

That's a genuinely strong CAN Bus foundation.

Ready to start Phase 1 — C# and .NET basics? Since you now have solid C embedded knowledge, C# will feel more familiar than you expect. We'll build toward eventually rewriting the CAN logger and decoder in C# — which is exactly the job stack you're targeting.start phase 1Phase 1 — C# and .NET Basics
Before we write any code, let me give you the big picture first.

How C# Relates to What You Already Know
You know C. That's a huge advantage. C# was inspired by C, so the syntax will feel familiar — but C# adds a lot on top:
C  (what you know)
│
└── C# builds on C syntax
    ├── same: if/else, for, while, functions, structs
    ├── adds: classes, objects, garbage collection
    ├── adds: built-in string handling
    ├── adds: exceptions instead of error codes
    └── adds: huge standard library (.NET)
You won't be starting from zero — you'll be extending what you know.

What is .NET?
C# is the language. .NET is the platform that runs it.
Your C# code
     │
     ▼
.NET Runtime  ←── handles memory, I/O, networking
     │
     ▼
Linux / Windows / Mac
Think of it like this:

C# = the language you write in
.NET = the engine that runs it + the library of tools

In embedded Linux work, you use .NET 6/7/8 which runs natively on Linux — including ARM devices like Raspberry Pi.

Check if .NET is Installed
In your Ubuntu terminal:
bashdotnet --version
If you see something like 8.0.xxx — you're ready.
If you see command not found — install it:
bashsudo apt-get update
sudo apt-get install -y dotnet-sdk-8.0
Then verify again:
bashdotnet --version

Tell me what version you see and we'll create your first C# project.do i have to learn c# if i want to expert in can busNo, you don't — CAN Bus and C# are two separate skills that happen to appear together in that specific job role.

The honest picture:
CAN Bus expertise is built with:

C (which you now know well)
Python (very common for CAN tooling)
SocketCAN / Linux (which you've done)
Protocol knowledge (frames, DBC, filters — all done)

C# is only needed if the specific company uses .NET as their application layer — like the job role you mentioned earlier (dotnet/C# + Embedded Linux + CAN Bus).

What You Already Have
After Phase 3 you can genuinely say in an interview:

✅ I understand CAN Bus protocol — frames, arbitration, filtering
✅ I've worked with SocketCAN on Linux
✅ I've written C programs that send, receive, filter, and log CAN frames
✅ I understand DBC signal decoding
✅ I've simulated real sensor nodes with periodic transmission

That's real CAN Bus knowledge — not just theory.

So Should You Learn C#?
Only if one of these is true:
ReasonLearn C#?You want that specific dotnet + CAN Bus job✅ YesYou want general embedded Linux work❌ Not neededYou want automotive diagnostics tools❌ Python is more commonYou want to work at Bosch/Continental/ZF❌ Mostly C/C++

My Honest Recommendation for You
Given your background — FPGA, TinyML, IMU, Bosch internship, embedded systems — your strongest job path is:
Embedded C/C++  +  Linux  +  CAN Bus  +  Sensor systems
That matches roles at companies like:

Bosch, Continental, ZF — automotive Tier 1
Sick, Beckhoff, Festo — industrial automation
Any company building sensor-heavy embedded products

C# is a nice addition but not your core strength or primary direction.

What do you want to do? I can:
A) Continue with C# since the original job role needs it
B) Skip C# and go deeper into embedded Linux + CAN (more C, Python-based CAN tools, real hardware setup)
C) Help you identify specific job roles that match your current skillset right nownow i want to learn embedded LinuxGreat choice — it fits your background perfectly. Before I build the roadmap, let me understand your current level.

Quick Assessment — 3 Questions
How comfortable are you with these Linux basics right now?
1. Basic Linux commands
Things like ls, cd, mkdir, cat, grep, chmod, ps, kill — do you use these regularly or only occasionally?
2. Shell scripting
Have you ever written a .sh script — loops, variables, conditions in bash?
3. Linux from your thesis/internship
At Bosch Sensortec or during your FPGA/IMU thesis work — did you use Linux? If yes, what did you do on it — compiling code, flashing firmware, reading sensor output, something else?
Answer these three and I'll build a roadmap that starts exactly where you are — no wasted time on things you already know.all answers are noNo problem — we start from zero. Actually this is the cleanest way to learn because we build everything properly from the ground up.

Good News First
You've already used Linux more than you think. Everything you did in Phase 3 — WSL2, terminal, nano, gcc, apt-get, modprobe, socket programming — that was all Linux. You're not a complete beginner, you just haven't learned it systematically yet.

Embedded Linux Roadmap — From Zero

Phase A — Linux Fundamentals
~2 months at 1–3 hrs/week
Before embedded, you need to be comfortable with Linux itself.
TopicWhat you'll learnFile systemHow Linux organizes files and directoriesBasic commandsNavigate, create, delete, copy, move filesFile permissionschmod, chown, users and groupsProcessesps, kill, top, background/foreground jobsText toolsgrep, cat, tail, nano, vim basicsShell scriptingAutomate tasks with bash scriptsPackage managementapt-get, installing and removing software

Phase B — Linux System Programming (C on Linux)
~2 months
You already started this in Phase 3 — sockets, read(), write(), ioctl(). Now we go deeper and broader.
TopicWhat you'll learnFile I/OOpen, read, write files from CProcessesfork(), exec(), process managementThreadspthreads — parallel executionIPCPipes, shared memory, message queuesSignalsHandle system events (you did SIGINT already)TimersPrecise timing for embedded tasksDevice files/dev/ — how Linux exposes hardware

Phase C — Embedded Linux Specific
~2–3 months
This is where your embedded background connects to Linux.
TopicWhat you'll learnCross compilationBuild on x86, run on ARMDevice treeHow Linux describes hardwareKernel modulesWrite your own Linux driverGPIO from LinuxControl hardware pinsSPI/I2C from LinuxTalk to sensors (connects to your IMU experience)systemd servicesRun your program automatically on bootYocto basicsBuild a custom Linux image for embedded hardware

Phase D — Putting It Together
~1–2 months
Build one complete project combining everything:
A C program running as a systemd service on embedded Linux that reads sensor data over SPI/I2C and logs it — with CAN Bus output. This directly combines your IMU background with everything you've learned.

Full Timeline
PhaseFocusDurationALinux fundamentals2 monthsBLinux system programming2 monthsCEmbedded Linux specific2–3 monthsDFinal project1–2 monthsTotal7–9 months
At 1–3 hrs/week this is realistic and solid — not rushed.

Where You Already Are
Before we even start you have credit for:
TopicStatusTerminal basics✅ You've been using itapt-get✅ Done in Phase 3nano editor✅ Done in Phase 3gcc compiler✅ Done in Phase 3Linux sockets in C✅ Done in Phase 3Signal handling✅ Done in Phase 3 (SIGINT)Kernel modules✅ Done (modprobe vcan)
So Phase A is partly done and Phase B has already started.

Ready to begin Phase A — Lesson 1?
We'll start with the Linux file system — how everything is organized, why it matters for embedded work, and the commands you'll use every single day.yes, startPhase A — Lesson 1
The Linux File System

Everything is a File
The most important concept in Linux:

Everything is a file — programs, hardware devices, network sockets, sensors, memory — Linux represents almost everything as a file you can read or write.

This is why in Phase 3 you could use read() and write() on a CAN socket — same functions you use for normal files. Same concept for SPI sensors, GPIO pins, serial ports — all files.

The File System Tree
Linux has one single tree starting from / (called root):
/                        ← root, top of everything
├── bin/                 ← essential programs (ls, cp, cat)
├── boot/                ← kernel and bootloader files
├── dev/                 ← device files (hardware as files)
│   ├── ttyS0            ← serial port
│   ├── spidev0.0        ← SPI device
│   └── vcan0            ← your CAN interface
├── etc/                 ← system configuration files
│   └── systemd/         ← service configurations
├── home/                ← user home directories
│   └── azad/            ← your personal files
├── lib/                 ← shared libraries
├── mnt/                 ← mount points for drives
├── proc/                ← running processes as files
│   ├── cpuinfo          ← CPU information
│   └── meminfo          ← memory information
├── sys/                 ← hardware info from kernel
│   └── class/
│       └── gpio/        ← GPIO pins as files
├── tmp/                 ← temporary files
├── usr/                 ← installed software
│   ├── bin/             ← user programs
│   └── include/         ← header files (like linux/can.h)
└── var/                 ← logs and variable data
    └── log/             ← system logs

Why This Matters for Embedded Linux
The folders you'll use most in embedded work:
FolderWhy you care/dev/Hardware devices — SPI, I2C, UART, GPIO/sys/Read hardware info, control GPIO pins/proc/Read CPU, memory, process information/etc/systemd/Register your program to run on boot/usr/include/Header files for C programming/var/log/System logs for debugging

Essential Navigation Commands
Let's learn these one by one with real practice.

1. pwd — Where am I?
bashpwd
Prints your current location. Try it now — you should see:
/home/azad
or something similar. This is your home directory — your personal space in Linux.

2. ls — What's here?
bashls
Lists files and folders in current directory.
More useful versions:
bashls -l        # detailed list with permissions and sizes
ls -la       # same but also shows hidden files (starting with .)
ls -lh       # human readable file sizes (KB, MB instead of bytes)
Try all three and notice the difference.

3. cd — Move around
bashcd /          # go to root
ls            # see everything at root level

cd /dev       # go to devices folder
ls            # see all device files

cd ~          # go back to your home directory (~ means home)
pwd           # confirm you're home
Try these now and tell me what you see inside /dev.

4. mkdir — Create a folder
bashcd ~                    # make sure you're home
mkdir can_projects      # create a folder for our work
ls                      # verify it was created
cd can_projects         # go into it
pwd                     # confirm location

5. touch — Create an empty file
bashtouch notes.txt         # creates empty file
ls -l                   # see it in the list

6. cp — Copy a file
bashcp notes.txt notes_backup.txt    # copy
ls                               # see both files

7. mv — Move or rename a file
bashmv notes_backup.txt old_notes.txt   # rename
ls                                  # see new name

8. rm — Delete a file
bashrm old_notes.txt      # delete file
ls                    # confirm it's gone
Be careful with rm — Linux has no recycle bin. Deleted = gone.

9. cat — Read a file
bashcat /proc/cpuinfo     # read CPU information
This reads the file /proc/cpuinfo which the kernel fills with your CPU details. Remember — even hardware info is a file in Linux.
Try also:
bashcat /proc/meminfo     # memory information

10. clear — Clean the terminal
bashclear
Clears the screen. Or press Ctrl+L — same thing.

Practice Exercise
Run all these commands in order and tell me what you see:
bash# 1. See where you are
pwd

# 2. Go to root and look around
cd /
ls

# 3. Go into /proc and read CPU info
cd /proc
cat cpuinfo

# 4. Go home and create a project folder
cd ~
mkdir embedded_linux
cd embedded_linux
pwd

# 5. Create some files
touch lesson1.txt
touch lesson2.txt
ls -l

# 6. Copy and rename
cp lesson1.txt lesson1_backup.txt
mv lesson2.txt lesson2_notes.txt
ls

# 7. Clean up one file
rm lesson1_backup.txt
ls

Key Concepts from Lesson 1
ConceptWhat to remember/Root — top of everything~Shortcut for your home directory/dev/Hardware as files/proc/Kernel and process info as filespwdWhere am Ils -laWhat's here including hidden filescdMove aroundcatRead a file

Run the practice exercise and paste what you see. Then we move to Lesson 2 — File Permissions which is critical for embedded work — device files, scripts, and executables all depend on permissions being set correctly.can you provide me all phase lessons, like how many lessons and their nameComplete Embedded Linux Roadmap — All Lessons

Phase A — Linux Fundamentals
Goal: Comfortable daily Linux usage
#LessonKey SkillsA1File System & Navigationpwd, ls, cd, mkdir, cp, mv, rm, catA2File Permissionschmod, chown, users, groups, rwxA3Text Toolsgrep, find, tail, head, wc, pipe operatorA4Processesps, top, kill, background jobs, & operatorA5Shell Scripting Basicsvariables, if/else, loops, functionsA6Shell Scripting Advancedarguments, file checks, cron jobsA7Package Managementapt-get, dpkg, searching, removing softwareA8Networking Basicsip, ping, ssh, netstat, curl

Phase B — Linux System Programming
Goal: Write C programs that interact with the Linux kernel
#LessonKey SkillsB1File I/O in Copen, read, write, close, lseekB2Error Handling in Cerrno, perror, strerror, defensive codingB3Processes in Cfork, exec, wait, exitB4Signals in Csignal, sigaction, common signalsB5Threads with pthreadspthread_create, mutex, synchronizationB6Pipes and IPCpipe, mkfifo, inter-process communicationB7Timers and Timingclock_gettime, nanosleep, interval timersB8Memory Managementmalloc, free, mmap, memory mapped files

Phase C — Embedded Linux Specific
Goal: Work with real hardware on embedded Linux systems
#LessonKey SkillsC1Device Files and /devcharacter devices, block devices, major/minorC2GPIO from Linux/sys/class/gpio, reading and writing pinsC3UART Serial from Linuxtermios, serial port configuration in CC4SPI from Linuxspidev, SPI transfers in CC5I2C from Linuxi2c-dev, reading I2C sensors in CC6Writing a Kernel Modulemodule_init, module_exit, basic driverC7Device Tree Basicswhat it is, how to read it, overlaysC8systemd Serviceswriting .service files, autostart on bootC9Cross Compilationtoolchain setup, build for ARM on x86C10Buildroot Basicsbuild a minimal custom Linux image

Phase D — Final Project
Goal: One complete real-world embedded Linux system
#LessonKey SkillsD1Project Designarchitecture, requirements, block diagramD2Sensor Reader Serviceread IMU/sensor data, systemd serviceD3CAN Bus Integrationconnect sensor data to CAN outputD4Data Loggercombine sensor + CAN + file loggingD5Final Build and Demopackage everything, document, present

Total — 31 Lessons
PhaseLessonsDurationA — Fundamentals8~2 monthsB — System Programming8~2 monthsC — Embedded Specific10~2–3 monthsD — Final Project5~1–2 monthsTotal31~7–9 months

Where You Already Have Credit
From Phase 3 (CAN Bus work) you've already covered material from:
LessonStatusA1 — File System & Navigation✅ Partially doneA7 — Package Management✅ apt-get doneB4 — Signals in C✅ SIGINT doneB7 — Timers and Timing✅ nanosleep doneB1 — File I/O in C✅ read/write doneC1 — Device Files✅ vcan0 doneC6 — Kernel Modules✅ modprobe done
So you're not starting from zero — you already have partial credit for 7 lessons.

You're currently on A1. Ready to continue?should i use this command on windows terminal or ubuntuAlways run these on the Ubuntu terminal (the WSL2 one), not the Windows terminal.

Quick way to tell them apart:
Windows PowerShell/CMD prompt looks like:
PS C:\Users\YourName>
Ubuntu (WSL2) prompt looks like:
mista@DESKTOP-EU8OFN9:~$

If you ever see a PS C:\> prompt, that's Windows — close it or open Ubuntu instead (Windows key → type Ubuntu → Enter).
All the commands from Lesson 1 (pwd, ls, cd, mkdir, etc.) only work correctly in the Ubuntu terminal, since they're Linux commands.@DESKTOP-EU8OFN9:~$ cat /proc/cpoinfo
cat: /proc/cpoinfo: No such file or directorySmall typo — cpoinfo should be cpuinfo.
bashcat /proc/cpuinfo
cpu not cpo — easy mistake to make!lesson 2(A2)Phase A — Lesson 2
File Permissions

Why Permissions Matter in Embedded Linux
In embedded work you'll constantly deal with permissions:

Your C program can't access /dev/spidev0.0 — permission denied
Your script won't execute — permission denied
Your systemd service can't read a config file — permission denied

Understanding permissions fixes 90% of these frustrating errors.

The Basic Concept
Every file in Linux has three questions:
Who can READ it?
Who can WRITE it?
Who can EXECUTE it?
And three groups of people:
Owner   → the user who created the file
Group   → a group of users
Others  → everyone else

Reading Permissions
Run this in your Ubuntu terminal:
bashls -l ~
You'll see something like:
-rwxr-xr-- 1 azad azad 1234 Jun 10 12:00 can_send
drwxr-xr-x 2 azad azad 4096 Jun 10 11:00 embedded_linux
-rw-r--r-- 1 azad azad  128 Jun 10 11:30 notes.txt
Let's decode that first column:
-  rwx  r-x  r--
│   │    │    │
│   │    │    └── Others permissions
│   │    └─────── Group permissions
│   └──────────── Owner permissions
└──────────────── File type (- = file, d = directory)

The Permission Letters
LetterMeaningOn a fileOn a directoryrreadcan read contentscan list contentswwritecan modify filecan create/delete files insidexexecutecan run as programcan enter with cd-no permissiondenieddenied

Permission Examples
rwxr-xr--
│││ │││ │││
│││ │││ └└└── Others: r-- = read only
│││ └└└────── Group:  r-x = read and execute
└└└────────── Owner:  rwx = read, write, execute
rw-r--r--   ← typical text file
rwxr-xr-x   ← typical program/script
rwx------   ← private, only owner can access

chmod — Change Permissions
chmod changes permissions. Two ways to use it:

Method 1 — Symbolic (easier to read)
bashchmod u+x script.sh    # give owner execute permission
chmod g+w file.txt     # give group write permission
chmod o-r secret.txt   # remove read from others
chmod a+r file.txt     # give everyone read (a = all)
Letters:

u = user (owner)
g = group
o = others
a = all three

Operators:

+ = add permission
- = remove permission
= = set exactly


Method 2 — Numeric (faster, used everywhere)
Each permission has a number:
r = 4
w = 2
x = 1
- = 0
Add them up for each group:
rwx = 4+2+1 = 7
r-x = 4+0+1 = 5
r-- = 4+0+0 = 4
--- = 0+0+0 = 0
So chmod 755 file means:
7 = rwx  → owner can read, write, execute
5 = r-x  → group can read and execute
5 = r-x  → others can read and execute
Common permission numbers you'll use constantly:
NumberPermissionsTypical use755rwxr-xr-xprograms, scripts644rw-r--r--text files, configs600rw-------private files777rwxrwxrwxopen to everyone700rwx------private programs

Practice Exercise
Run all these in Ubuntu and observe what changes:
bash# 1. Go home and create a test file
cd ~
touch test_permissions.txt
ls -l test_permissions.txt

# 2. Check default permissions
ls -l test_permissions.txt
# should show -rw-r--r-- (644)

# 3. Remove all permissions
chmod 000 test_permissions.txt
ls -l test_permissions.txt
# should show ----------

# 4. Try to read it
cat test_permissions.txt
# should get: Permission denied

# 5. Give yourself read permission back
chmod 600 test_permissions.txt
cat test_permissions.txt
# works now (file is empty but no error)

# 6. Create a script and make it executable
touch my_script.sh
echo '#!/bin/bash' > my_script.sh
echo 'echo Hello from my script' >> my_script.sh
ls -l my_script.sh
# shows -rw-r--r-- (not executable yet)

# 7. Try to run it without execute permission
./my_script.sh
# Permission denied

# 8. Add execute permission
chmod 755 my_script.sh
ls -l my_script.sh
# now shows -rwxr-xr-x

# 9. Run it
./my_script.sh
# Hello from my script

chown — Change Owner
bashchown azad file.txt           # change owner to azad
chown azad:students file.txt  # change owner and group
You'll use this in embedded work when a service runs as a different user and needs access to a device file.

Real Embedded Example
When you connect a sensor to Linux — say an I2C sensor — its device file appears as:
crw-rw---- 1 root i2c 89, 1 Jun 10 /dev/i2c-1
Your program running as azad can't access it because:

Owner is root
Group is i2c
Others have no permissions (---)

Fix options:
bash# Option 1 — add yourself to the i2c group
sudo usermod -aG i2c azad

# Option 2 — change permissions temporarily
sudo chmod 666 /dev/i2c-1
This exact situation comes up constantly in embedded Linux work — you'll remember this lesson every time.

Summary
Permission string:  - r w x r - x r - -
                    │ │ │ │ │ │ │ │ │ │
                    │ └─┴─┘ └─┴─┘ └─┴─┘
                    │ owner  group others
                    │
                    └── file type (- or d)

chmod 755 = rwxr-xr-x = owner all, group/others read+execute
chmod 644 = rw-r--r-- = owner read+write, others read only
chmod 600 = rw------- = owner only

Run the practice exercise and paste what you see. Then we move to Lesson A3 — Text Tools (grep, find, tail, pipe) which are essential for reading log files and searching through embedded system output.can you give me a excel file of this Complete Embedded Linux Roadmap — All Lessons, also add one column more where i can track finish or ongoing something like thatRan 4 commands, viewed a file, created a fileRan 4 commands, viewed a file, created a fileHere's your tracker — all 31 lessons across Phases A–D, color-coded by phase, with a Status column (Not Started / Ongoing / Finished) that has a dropdown for easy updating. I marked A1, A2, A7, B1, B4, B7, C1, C6 based on what you've already covered — feel free to adjust as needed.Embedded linux roadmapSpreadsheet · XLSX file type (- or d) what does it mean, can you give me examplesGood question — that's the very first character in ls -l output, and it tells you what kind of item you're looking at, before you even read the permissions.

The Two You'll See Most
-  →  regular file
d  →  directory (folder)

Examples
bashls -l ~
-rw-r--r--  1 azad azad  128 Jun 10 notes.txt
drwxr-xr-x  2 azad azad 4096 Jun 10 embedded_linux
-rwxr-xr-x  1 azad azad 1234 Jun 10 my_script.sh
Breaking down the first character of each:
-rw-r--r--   →  "-" = regular file        (notes.txt)
drwxr-xr-x   →  "d" = directory            (embedded_linux folder)
-rwxr-xr-x   →  "-" = regular file         (my_script.sh — still a file, just executable)
Notice my_script.sh is still a file (-), not a directory — execute permission doesn't change the file type, it just means it can be run.

Other File Types You'll Encounter in Embedded Linux
This is where it gets relevant to your work — /dev/ is full of different types:
bashls -l /dev/ttyS0
ls -l /dev/vcan0 2>/dev/null
ls -l /dev/sda
SymbolTypeExample-Regular filenotes.txtdDirectory/home/azad/cCharacter device/dev/ttyS0 (serial port)bBlock device/dev/sda (hard disk)lSymbolic linkshortcut to another filesSocketused for inter-process communicationpNamed pipeFIFO, used for IPC

Why Character vs Block Matters for You
This connects directly to your embedded work:
c → character device  → data flows one byte at a time
                         (serial ports, sensors, /dev/vcan0-like interfaces)

b → block device       → data flows in fixed-size blocks
                         (hard drives, SD cards, USB storage)
When you eventually work with SPI or I2C sensors in Phase C, you'll see things like:
bashls -l /dev/spidev0.0
crw-rw---- 1 root spi 153, 0 Jun 10 /dev/spidev0.0
That c at the start tells you immediately — this is a character device, data comes through byte by byte, exactly how you'd read sensor values continuously.

Quick Way to Remember
- = ordinary file you'd open in a text editor
d = a folder, you can cd into it
c = hardware that streams data (sensors, serial, CAN-like)
b = storage device (disks, SD cards)
l = a shortcut pointing elsewhere
Try running ls -l /dev/ | head -20 in your Ubuntu terminal — you'll see a mix of c, b, and other types right away. Want to try that and see what's in your /dev/ folder?go for lesson 3Phase A — Lesson 3
Text Tools

Why This Lesson Matters
In embedded Linux work you'll constantly need to search through log files, filter sensor output, and process text data — often massive files you can't just scroll through manually. These tools are how you do that efficiently.

cat — You Already Know This One
Quick recap:
bashcat /proc/cpuinfo    # print whole file
Useful for small files, but useless for huge log files — it dumps everything at once.

head and tail — See Just the Beginning or End
bashhead -n 5 /proc/cpuinfo     # first 5 lines
tail -n 5 /proc/cpuinfo     # last 5 lines
This is huge for log files. Imagine a log file with 100,000 lines — you usually only care about the most recent entries.
The most important flag for embedded/debugging work:
bashtail -f can_log.txt
-f means follow — it keeps watching the file and prints new lines as they're added, live. This is exactly how you'd watch your can_logger output in real time from a second terminal while it's running.
Try it now:
bashtail -f /home/azad/can_log.txt
(Press Ctrl+C to stop watching)

grep — Search Inside Files
grep finds lines containing a specific word or pattern. This is one of the most-used commands in all of Linux.
bashgrep "error" logfile.txt          # find lines containing "error"
grep -i "error" logfile.txt       # case-insensitive search
grep -n "error" logfile.txt       # show line numbers too
grep -c "error" logfile.txt       # count how many matches
grep -r "error" /var/log/         # search recursively through folders
Real example with your CAN logger:
bashgrep "0x101" can_log.txt
This pulls out only the RPM sensor lines from your log file, ignoring everything else — instead of scrolling through hundreds of lines manually.
bashgrep -c "0x300" can_log.txt
This tells you exactly how many tire pressure frames were logged.

find — Search for Files (Not Content, the Files Themselves)
bashfind /home/azad -name "*.c"          # find all .c files
find /home/azad -name "can_*"        # find files starting with "can_"
find / -name "vcan*" 2>/dev/null     # find anything related to vcan
PartMeaningfind /home/azadsearch starting from this folder-name "*.c"match files ending in .c2>/dev/nullhide "permission denied" error messages
* is a wildcard — matches anything. *.c means "any name ending in .c".

wc — Count Lines, Words, Characters
bashwc -l can_log.txt        # count lines
wc -w can_log.txt        # count words
wc -c can_log.txt        # count characters/bytes
Real use case: Quickly check how many frames your logger captured:
bashwc -l can_log.txt

The Pipe Operator | — Connecting Commands Together
This is the most powerful concept in this lesson. The pipe takes the output of one command and feeds it as the input to the next command.
bashcommand1 | command2
Think of it like a pipe carrying water from one tap to the next.

Real Examples Using What You Already Built
Count how many RPM frames were logged:
bashgrep "0x101" can_log.txt | wc -l

grep finds all RPM lines
| sends those lines to wc -l
wc -l counts how many lines it received


Watch only RPM frames live, as they're logged:
bashtail -f can_log.txt | grep "0x101"

tail -f streams new lines as they appear
grep filters to show only RPM frames


See the last 3 tire pressure readings:
bashgrep "0x300" can_log.txt | tail -n 3

List all your .c files, count them:
bashfind ~ -name "*.c" | wc -l

Practice Exercise
Make sure you're in your project folder with can_log.txt from earlier lessons:
bashcd ~
ls
Then run these one by one:
bash# 1. See last 10 lines of your log
tail can_log.txt

# 2. Find all RPM frames
grep "0x101" can_log.txt

# 3. Count how many RPM frames exist
grep "0x101" can_log.txt | wc -l

# 4. Count total lines in the log
wc -l can_log.txt

# 5. Find all your C files
find ~ -name "*.c"

# 6. Combine grep and head — show only first 3 tire pressure readings
grep "0x300" can_log.txt | head -n 3

Summary Table
CommandPurposehead -n N fileShow first N linestail -n N fileShow last N linestail -f fileWatch file live as it growsgrep "text" fileSearch for lines containing textgrep -iCase-insensitive searchgrep -cCount matching linesfind path -name "pattern"Search for files by namewc -lCount lines| (pipe)Send output of one command into another

Run the practice exercise — if you don't have can_log.txt anymore (since WSL2 sessions can reset), let me know and we'll regenerate it quickly using your can_logger and can_periodic programs from Phase 3. Then we move to Lesson A4 — Processes, where you'll learn ps, top, and kill — essential for managing running embedded programs.next lessonViewed memory editsViewed memory editsPhase A — Lesson 4
Processes

What is a Process?
Every time you run a program, Linux creates a process — a running instance of that program with its own memory, its own ID number, and its own lifecycle.
Program on disk        →  just a file (e.g. ./can_logger)
Program running         →  a PROCESS with a PID (Process ID)
You've already created processes without realizing it — every time you ran ./can_send, ./can_logger, candump vcan0 — each was a separate process.

ps — See Running Processes
bashps
Shows processes running in your current terminal session only:
  PID TTY          TIME CMD
 1234 pts/0    00:00:00 bash
 5678 pts/0    00:00:00 ps
ColumnMeaningPIDProcess ID — unique numberTTYWhich terminal it's running inTIMECPU time usedCMDThe command/program name

See ALL processes on the system
bashps aux
This shows every process from every user — much longer list:
USER   PID  %CPU %MEM    VSZ   RSS TTY   STAT START   TIME COMMAND
azad     1   0.0  0.1   1234   800 ?     Ss   10:00   0:00 /sbin/init
azad   245   0.0  0.5   8932  4200 pts/0 S    10:05   0:01 ./can_logger
FlagMeaningashow processes from all usersushow detailed info (CPU%, memory%)xinclude processes without a terminal

Find a specific process
Combine ps with grep — remember the pipe from last lesson:
bashps aux | grep can_logger
This searches the entire process list for anything matching can_logger — much faster than scrolling through everything.

top — Live Process Monitor
bashtop
This opens a live, constantly updating view of all processes — like Windows Task Manager but in the terminal.
PID  USER  %CPU  %MEM  TIME+   COMMAND
245  azad   2.3   0.5  0:01.20 can_logger
312  azad   0.1   0.2  0:00.05 bash
Press q to quit top and return to the terminal.
Useful keys inside top:

q — quit
k — kill a process (it'll ask for PID)
M — sort by memory usage
P — sort by CPU usage


Background and Foreground Jobs
Right now when you run a program, it takes over your terminal — you can't type anything else until it finishes or you press Ctrl+C. That's called running in the foreground.
You can send it to the background instead, freeing up your terminal:

Run something in the background with &
bash./can_logger &
Output:
[1] 4521

[1] = job number
4521 = PID

Your terminal is immediately free again — you can keep typing other commands while can_logger keeps running behind the scenes.

jobs — See background jobs in this terminal
bashjobs
[1]+  Running    ./can_logger &

Bring a background job back to foreground
bashfg
This brings job [1] back to the foreground — now Ctrl+C will work on it again.

Send a foreground job to background without stopping it
If something is already running in the foreground:

Press Ctrl+Z — this pauses it
Type bg — resumes it in the background


kill — Stop a Process
Every process can be stopped using its PID:
bashkill 4521
This sends a polite "please stop" signal (SIGTERM) — same as Ctrl+C but you can do it from a different terminal window.
If a process won't stop (rare, but happens):
bashkill -9 4521
-9 is SIGKILL — a forceful, immediate stop. Use this only when kill alone doesn't work.

Connecting to What You Already Know
Remember this from your can_logger.c?
csignal(SIGINT, handle_exit);
SIGINT is exactly what Ctrl+C sends to a process. Now you know:
kill 4521        → sends SIGTERM (default)
kill -2 4521      → sends SIGINT  (same as Ctrl+C)
kill -9 4521      → sends SIGKILL (force kill, can't be caught)
Ctrl+C            → sends SIGINT directly to foreground process
This is why your logger needed a signal handler — it was specifically catching SIGINT to shut down cleanly instead of dying instantly.

Practice Exercise
bash# 1. Start your CAN logger in the background
cd ~
./can_logger &

# 2. Check it's running
jobs
ps aux | grep can_logger

# 3. Note the PID number, then send some CAN frames
cansend vcan0 101#1111
cansend vcan0 300#2222

# 4. Check the log file is being written even though logger runs in background
tail can_log.txt

# 5. Bring it to foreground
fg
# Press Ctrl+C to stop it normally

# 6. Try again but kill it from another terminal instead
./can_logger &
jobs
# note the PID
kill <PID>     # replace <PID> with the actual number

# 7. Confirm it stopped
jobs
ps aux | grep can_logger

Real Embedded Use Case
In real embedded Linux systems, your sensor-reading program, your CAN logger, your data processor — they all run simultaneously as background processes or services. Understanding ps, top, kill, and background jobs is exactly how you'd monitor and manage a running embedded system, whether you're debugging on your dev machine or checking what's running on deployed hardware.

Summary Table
CommandPurposepsList processes in current terminalps auxList all processes on systemps aux | grep nameFind a specific processtopLive process monitorcommand &Run in backgroundjobsList background jobsfgBring job to foregroundCtrl+Z then bgPause then resume in backgroundkill PIDStop a process gracefullykill -9 PIDForce kill a process

Try the practice exercise and paste what you see. Once confirmed, we move to Lesson A5 — Shell Scripting Basics, where you'll start automating multi-step tasks like the ones you've been doing manually — variables, if/else, and loops in bash.next lessonPhase A — Lesson 5
Shell Scripting Basics

What is a Shell Script?
A shell script is a text file containing Linux commands that run automatically in sequence — like a program, but written in bash instead of C.
Instead of typing 10 commands manually every time, you write them once in a script and run it with one command.

Why This Matters for Embedded Linux
In embedded work you'll use scripts constantly:

Automate setting up vcan0 on boot
Run your CAN logger and periodic sender together with one command
Check if a sensor device exists before starting a program
Process and filter log files automatically


Your First Script
You already wrote one in Lesson A2 without realizing:
bashtouch my_script.sh
echo '#!/bin/bash' > my_script.sh
echo 'echo Hello from my script' >> my_script.sh
chmod 755 my_script.sh
./my_script.sh
Let's now learn each building block properly.

The Shebang Line
Every script starts with:
bash#!/bin/bash
This tells Linux which interpreter to use. #!/bin/bash means "run this script with bash". Without it Linux doesn't know how to execute the file.

Variables
bash#!/bin/bash

# Define variables — no spaces around =
name="Azad"
interface="vcan0"
count=5

# Use variables with $
echo "Hello $name"
echo "Using interface: $interface"
echo "Count is: $count"
Key rules:

No spaces around = when setting a variable
Always use $ when reading a variable
Use quotes around strings


User Input
bash#!/bin/bash
echo "Enter your name:"
read username
echo "Welcome $username"
read waits for the user to type something and stores it in a variable.

If / Else
bash#!/bin/bash

temperature=95

if [ $temperature -gt 100 ]; then
    echo "WARNING: Engine overheating!"
elif [ $temperature -gt 90 ]; then
    echo "Temperature high but normal"
else
    echo "Temperature OK"
fi
Comparison operators for numbers:
Operator        Meaning
-eq             equal to
-ne             not equal to
-gt             greater than
-lt             less than
-ge             greater than or equal
-le             less than or equal

if [ "$name" = "Azad" ]; then
    echo "Welcome back"
fi

#!/bin/bash

# Loop through a list
for sensor in RPM Temperature Pressure; do
    echo "Reading sensor: $sensor"
done

# Loop through numbers
for i in 1 2 3 4 5; do
    echo "Frame number: $i"
done

#!/bin/bash

count=1

while [ $count -le 5 ]; do
    echo "Sending frame $count"
    count=$((count + 1))   # increment count
done
#!/bin/bash

# Define function
send_can_frame() {
    local id=$1      # first argument
    local data=$2    # second argument
    echo "Sending frame ID=$id Data=$data"
    cansend vcan0 $id#$data
}

# Call function
send_can_frame 101 AABBCCDD
send_can_frame 300 1122
send_can_frame 200 DEADBEEF

nano setup_can.sh
#!/bin/bash

# ── Configuration ─────────────────────────────
INTERFACE="vcan0"

# ── Functions ─────────────────────────────────
setup_interface() {
    echo "Setting up $INTERFACE..."
    sudo modprobe vcan
    sudo ip link add dev $INTERFACE type vcan
    sudo ip link set up $INTERFACE
}

check_interface() {
    if ip link show $INTERFACE > /dev/null 2>&1; then
        echo "$INTERFACE is already up"
        return 1
    else
        return 0
    fi
}

send_test_frames() {
    echo "Sending test frames..."
    for id in 101 200 300; do
        cansend $INTERFACE ${id}#DEADBEEF
        echo "  Sent frame ID: 0x${id}"
    done
}

# ── Main ──────────────────────────────────────
echo "=== CAN Bus Setup Script ==="

if check_interface; then
    setup_interface
    if ip link show $INTERFACE > /dev/null 2>&1; then
        echo "$INTERFACE is UP and ready"
        send_test_frames
        echo "Setup complete!"
    else
        echo "ERROR: Failed to bring up $INTERFACE"
        exit 1
    fi
fi

chmod 755 setup_can.sh
./setup_can.sh

check_interface() {
    if ...; then
        return 1   # interface exists
    else
        return 0   # interface doesn't exist
    fi
}

nano exercise1.sh
#!/bin/bash
# Send CAN frames with increasing RPM values
interface="vcan0"
rpm=800

echo "Simulating RPM sensor..."

for i in 1 2 3 4 5; do
    # Convert rpm to hex
    hex_rpm=$(printf '%04X' $rpm)
    echo "RPM: $rpm → 0x$hex_rpm"
    cansend $interface "101#$hex_rpm"
    rpm=$((rpm + 200))
done

echo "Done."
chmod 755 exercise1.sh
./exercise1.sh

nano exercise2.sh
#!/bin/bash
echo "Enter a temperature value (degrees C):"
read temp

if [ $temp -gt 100 ]; then
    echo "CRITICAL: Engine overheating! ($temp°C)"
elif [ $temp -gt 90 ]; then
    echo "WARNING: Temperature high ($temp°C)"
elif [ $temp -gt 60 ]; then
    echo "OK: Normal operating temperature ($temp°C)"
else
    echo "LOW: Engine warming up ($temp°C)"
fi
chmod 755 exercise2.sh
./exercise2.sh

Concept         Syntax
Shebang         #!/bin/bash
Variable        name="value"
Use variable     $name
Read input      read varname
If/else         if [ condition ]; then ... fi
For loop        for x in list; do ... done
While loop      while [ condition ]; do ... done
Function        funcname() { ... }
Math$((a + b))
Silence output      > /dev/null 2>&1
Exit with error     exit 1
*/

/*
Phase A — Lesson 6
Shell Scripting Advanced

Right now you run your scripts like:
bash./setup_can.sh
But real scripts accept input directly on the command line:
./setup_can.sh vcan0 500
./send_frames.sh 101 DEADBEEF 5
#!/bin/bash
echo "First argument:  $1"
echo "Second argument: $2"
echo "Third argument:  $3"
echo "All arguments:   $@"
echo "How many args:   $#"
Variable        Meaning
$1 $2 $3        Individual arguments
$@              All arguments as a list
$#              Number of arguments
$0              Script name itself

#!/bin/bash

# Check argument count
if [ $# -ne 2 ]; then
    echo "Usage: $0 <can_id> <data>"
    echo "Example: $0 101 DEADBEEF"
    exit 1
fi

can_id=$1
data=$2

echo "Sending frame ID=$can_id Data=$data"
cansend vcan0 ${can_id}#${data}

./send.sh           # forgot arguments
Usage: ./send.sh <can_id> <data>
Example: ./send.sh 101 DEADBEEF
#!/bin/bash

# Check if a regular file exists
if [ -f "can_log.txt" ]; then
    echo "Log file exists"
else
    echo "Log file not found"
fi

# Check if a directory exists
if [ -d "/home/azad/can_projects" ]; then
    echo "Project folder exists"
fi

# Check if a device file exists
if [ -e "/dev/ttyS0" ]; then
    echo "Serial port available"
fi

# Check if a file is executable
if [ -x "./can_logger" ]; then
    echo "Logger is executable"
fi

-f      file exists and is a regular file
-d      directory exists
-e      anything exists (file, device, dir)
-x      file is executable
-r      file is readable
-w      file is writable
-s      file exists and is not empty

Every Linux command returns an exit code:

0 = success
Non-zero = failure

#!/bin/bash

# $? holds the exit code of the last command
ip link show vcan0 > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "vcan0 is up"
else
    echo "vcan0 is not available"
fi

# Even cleaner way
if ip link show vcan0 > /dev/null 2>&1; then
    echo "vcan0 is up"
else
    echo "vcan0 is not available"
fi

set -e — Stop on Any Error
#!/bin/bash
set -e

crontab -e
# ┌──── minute (0-59)
# │  ┌──── hour (0-23)
# │  │  ┌──── day of month (1-31)
# │  │  │  ┌──── month (1-12)
# │  │  │  │  ┌──── day of week (0-6, 0=Sunday)
# │  │  │  │  │
# *  *  *  *  *  command

Example
# Run every minute
* * * * * /home/azad/setup_can.sh

# Run every day at 8am
0 8 * * * /home/azad/backup_logs.sh

# Run every hour
0 * * * * /home/azad/check_sensors.sh

# Run every 5 minutes
(*)/5 * * * * /home/azad/log_status.sh

# Run at boot
@reboot /home/azad/setup_can.sh

nano can_tool.sh
#!/bin/bash
set -e

# ── Configuration ──────────────────────────────
INTERFACE="vcan0"
LOG_DIR="$HOME/can_logs"
LOGFILE="$LOG_DIR/can_$(date +%Y%m%d_%H%M%S).txt"

# ── Functions ──────────────────────────────────
usage() {
    echo "Usage: $0 <command> [options]"
    echo ""
    echo "Commands:"
    echo "  setup              Setup vcan0 interface"
    echo "  send <id> <data>   Send a single CAN frame"
    echo "  log <seconds>      Log CAN traffic for N seconds"
    echo ""
    echo "Examples:"
    echo "  $0 setup"
    echo "  $0 send 101 DEADBEEF"
    echo "  $0 log 10"
    exit 1
}

check_interface() {
    if ! ip link show $INTERFACE > /dev/null 2>&1; then
        echo "ERROR: $INTERFACE is not up. Run '$0 setup' first."
        exit 1
    fi
}

setup_can() {
    echo "Setting up $INTERFACE..."
    sudo modprobe vcan
    if ! ip link show $INTERFACE > /dev/null 2>&1; then
        sudo ip link add dev $INTERFACE type vcan
    fi
    sudo ip link set up $INTERFACE
    echo "$INTERFACE is ready."
}

send_frame() {
    if [ $# -ne 2 ]; then
        echo "ERROR: send needs <id> and <data>"
        echo "Example: $0 send 101 DEADBEEF"
        exit 1
    fi
    check_interface
    local id=$1
    local data=$2
    cansend $INTERFACE ${id}#${data}
    echo "Sent: ID=0x${id} Data=${data}"
}

log_traffic() {
    if [ $# -ne 1 ]; then
        echo "ERROR: log needs <seconds>"
        echo "Example: $0 log 10"
        exit 1
    fi
    check_interface
    local duration=$1

    # Create log directory if not exists
    if [ ! -d "$LOG_DIR" ]; then
        mkdir -p "$LOG_DIR"
        echo "Created log directory: $LOG_DIR"
    fi

    echo "Logging $INTERFACE for ${duration}s → $LOGFILE"
    timeout $duration candump $INTERFACE > $LOGFILE || true
    local count=$(wc -l < $LOGFILE)
    echo "Done. Captured $count frames → $LOGFILE"
}

# ── Main ───────────────────────────────────────
if [ $# -eq 0 ]; then
    usage
fi

command=$1
shift    # remove first argument, rest are now $1 $2 etc

case $command in
    setup)
        setup_can
        ;;
    send)
        send_frame $@
        ;;
    log)
        log_traffic $@
        ;;
    *)
        echo "ERROR: Unknown command '$command'"
        usage
        ;;
esac
chmod 755 can_tool.sh
./can_tool.sh setup
./can_tool.sh send 101 DEADBEEF
./can_periodic &
./can_tool.sh log 5

Output:
Logging vcan0 for 5s → /home/azad/can_logs/can_20240610_143022.txt
Done. Captured 52 frames → /home/azad/can_logs/can_20240610_143022.txt


case $command in
    setup)
        setup_can
        ;;
    send)
        send_frame $@
        ;;
    *)
        echo "Unknown command"
        ;;
esac

command=$1
shift
timeout 5 candump vcan0
LOGFILE="can_$(date +%Y%m%d_%H%M%S).txt"
can_20240610_143022.txt

*/

/*
Phase A — Lesson 7
Package Management
sudo apt-get update
sudo apt-get install can-utils
sudo apt-get install gcc
sudo apt-get install -y can-utils    # -y skips the yes/no confirmation
sudo apt-get remove can-utils
sudo apt-get purge can-utils
sudo apt-get upgrade
apt-cache search can
apt-cache search serial port
apt-cache search i2c
apt-cache search can | grep -i bus
apt-cache show can-utils
dpkg -l
dpkg -l | grep can
dpkg -l | grep gcc
dpkg -l | grep can-utils
ii  can-utils  2020.02.04  amd64  CAN bus utilities
ii = installed and working. rc = removed but config files remain.
sudo apt-get autoremove
# Build tools
sudo apt-get install build-essential    # gcc, make, and other essentials
sudo apt-get install cmake              # build system

# Hardware interfaces
sudo apt-get install i2c-tools          # I2C utilities
sudo apt-get install python3-smbus      # I2C from Python

# Debugging
sudo apt-get install gdb                # C debugger
sudo apt-get install strace             # trace system calls

# CAN Bus (already installed)
sudo apt-get install can-utils

# Network tools (preview of next lesson)
sudo apt-get install net-tools          # ifconfig and others
sudo apt-get install nmap               # network scanner
# 1. Update package list
sudo apt-get update

# 2. Search for i2c related packages
apt-cache search i2c

# 3. Get info about can-utils
apt-cache show can-utils

# 4. Confirm can-utils is installed
dpkg -l | grep can-utils

# 5. Check what gcc version you have
gcc --version

# 6. Install build-essential if not already there
sudo apt-get install -y build-essential

# 7. Clean up
sudo apt-get autoremove
Command                     Purpose
sudo apt-get update         Refresh package list
sudo apt-get install pkg    Install a package
sudo apt-get remove pkg     Remove a package
sudo apt-get purge pkg      Remove including config files
sudo apt-get upgrade        Update all installed packages
sudo apt-get autoremove     Remove unused dependencies
apt-cache search keyword    Search available packages
apt-cache show pkg          Show package details
dpkg -l                     List installed packages
dpkg -l | grep pkg          Check if specific package installed

*/

/*
Lesson 8
Networking Basics

# Show all interfaces
ip link show

# Show IP addresses
ip addr show

# Show specific interface
ip addr show eth0
output
2: eth0: <BROADCAST,MULTICAST,UP,LOWER_UP>
    link/ether 00:11:22:33:44:55
    inet 192.168.1.100/24 brd 192.168.1.255

eth0          →  interface name
UP,LOWER_UP   →  interface is active
00:11:22:...  →  MAC address (hardware address)
192.168.1.100 →  IP address
/24           →  subnet mask (means 255.255.255.0)

ping google.com          # ping by hostname
ping 192.168.1.1         # ping by IP address
ping -c 4 google.com     # send only 4 packets then stop
PING google.com (142.250.74.46)
64 bytes from 142.250.74.46: icmp_seq=1 ttl=115 time=12.3 ms
64 bytes from 142.250.74.46: icmp_seq=2 ttl=115 time=11.8 ms

Field           Meaning
icmp_seq        Packet sequence number
ttl             Time to live — how many network hops
time            Round trip time in milliseconds


ssh azad@192.168.1.50       # connect to device at that IP
ssh pi@raspberrypi.local    # connect to Raspberry Pi by hostname

exit

# Copy file TO remote device
scp can_logger pi@192.168.1.50:/home/pi/

# Copy file FROM remote device
scp pi@192.168.1.50:/home/pi/can_log.txt ./

# Copy entire folder
scp -r can_projects/ pi@192.168.1.50:/home/pi/

ss -tuln

Flag            Meaning
-t              TCP connections
-u              UDP connections
-l              listening ports only
-n              show numbers not names

Netid  State   Recv-Q  Send-Q  Local Address:Port
tcp    LISTEN  0       128     0.0.0.0:22
tcp    LISTEN  0       128     0.0.0.0:80

# Simple GET request
curl https://google.com

# Get just the response headers
curl -I https://google.com

# POST data to a server (like sending sensor data)
curl -X POST https://httpbin.org/post \
     -H "Content-Type: application/json" \
     -d '{"sensor":"RPM","value":1500}'

# Save response to a file
curl -o response.txt https://httpbin.org/get

wget https://example.com/file.tar.gz

hostname              # your device's name on the network
hostname -I           # your IP address
nslookup google.com   # look up IP address of a hostname

# 1. Find your device's IP
ping raspberrypi.local -c 2

# 2. SSH into it
ssh pi@192.168.1.50

# 3. On the device — check network is up
ip addr show

# 4. Exit back to your PC
exit

# 5. Copy your CAN logger to the device
scp can_logger pi@192.168.1.50:/home/pi/

# 6. SSH back in and run it
ssh pi@192.168.1.50
./can_logger &

# 1. See all your network interfaces
ip addr show

# 2. Check your IP address
hostname -I

# 3. Ping Google — confirm internet works
ping -c 4 google.com

# 4. Ping your WSL2 host (Windows machine)
ping -c 2 $(hostname -I | awk '{print $1}' | sed 's/\.[0-9]*$/.1/')

# 5. See active connections
ss -tuln

# 6. Make an HTTP request
curl -s https://httpbin.org/get | head -20

# 7. Send simulated sensor data
curl -X POST https://httpbin.org/post \
     -H "Content-Type: application/json" \
     -d '{"sensor":"RPM","value":1500,"unit":"rpm"}'

Command             Purpose
ip addr show        Show IP addresses of all interfaces
ip link show        Show network interfaces and status
ping -c N host      Test if host is reachable
ssh user@host       Connect to remote Linux device
scp file user@host:path Copy file to remote device
scp user@host:file .    Copy file from remote device
ss -tuln             Show active network connections
curl url            Make HTTP request
curl -X POST -d data url   Send data to server
wget url                Download a file
hostname -I             Show your IP address
*/