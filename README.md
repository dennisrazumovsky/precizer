<p width="100%" height="100%">
<img width="20%" height="20%" src="icon/micrometer.svg">
</p>

# Precizer
This program is distributed under the [CC0 (Creative Commons Share Alike) license](https://creativecommons.org/publicdomain/zero/1.0/). The author is not responsible for any use of the source code or the entire program. Anyone who uses the code or the program uses it at their own risk and responsibility.

Application Author: [Dennis Razumovsky](https://github.com/dennisrazumovsky)

## TL;DR

**precizer** is a CLI application designed to verify the integrity of files after synchronization. The program recursively traverses directories and creates a database of files and their checksums, followed by a quick comparison.

**precizer** specializes in managing vast file systems. The program identifies synchronization errors by cross-referencing data and checksums from various sources. Alternatively, it can be used to crawl historical changes by comparing databases from the same sources over different times.

## SIMPLE EXAMPLE

Assuming there are two hosts with large disks and identical contents mounted in /mnt1 and /mnt2 accordingly. The general task is to check whether the contents are identical or if there are any differences.

1. Run the program on the first machine with host name, for example “host1”:

```sh
precizer --progress /mnt1
```

The program recursively traverses all directories starting from /mnt1 and the host1.db database will be created in the current directory. The _--progress_ option visualizes progress and will show the amount of space and the number of files being examined.

2. Run the program on a second machine with a host name, for example host2:

```sh
precizer --progress /mnt2
```

As a result, the host2.db database will be created in the current directory.

3. Transfer the host1.db and host2.db files to either machine and run the program with the appropriate parameters to compare the databases:

```sh
precizer --compare host1.db host2.db
```

The following information will be displayed on the screen:
* Which files are missing on “host1” but present on “host2” and vice versa.
* For which files, present on both hosts, the checksums do NOT match.

Note that **precizer** writes only relative paths to the database. The example file “/mnt1/abc/def/aaa.txt” will be written to the database as “abc/def/aaa.txt” without _/mnt1_. The same thing will happen with the file “/mnt2/abc/def/aaa.txt”. Despite different mount points and different sources the files can be compared with each other under the same names “abc/def/aaa.txt” with the corresponding checksums.

## TECHNICAL DETAILS

Consider a scenario where there is a primary disk storage and a backup copy. For example, this could be a data center storage and its Disaster Recovery. Periodic synchronization transpires from the main storage to the DR storage, but due to the huge volumes of data, it's likely that synchronization does not occur on a byte-by-byte basis, but by calculating changes among the metadata of files on the file system. In such cases, the file size and modification time are taken into account, but the changed contents are not examined byte by byte. This makes sense because there are usually good communication channels between the primary data center and the backup Disaster Recovery center, but full byte-by-byte synchronization can be impractical due to the time it takes. Tools such as rsync allows to synchronize using both methods: File System changes and byte-by-byte comparison, but they have one serious drawback: the state is not saved between sessions. Let's look at what this means in the scenario:

* Given servers “A” and “B” (main data center and backup Disaster Recovery)
* Some files have changed on server “A”.
* The rsync algorithm identified them due to the changed size and modification time of the file of File System and synchronized them to server “B”.
* During synchronization, multiple communication failures occurred between the main data center and Disaster Recovery.
* To check data integrity (equivalence of stored files on “A” and “B” bytes to bytes), rsync is typically used again with byte-by-byte comparison enabled. In that case:
* rsync runs on server “A” in _--checksum_ mode and during one session tries to calculate checksums sequentially first on “A” and then on “B”.
* This process takes an incredibly long time for large disk arrays
* Since rsync does not allow saving the state of previously calculated checksums between sessions, a number of technical difficulties arise. Namely:
* If the connection is lost, rsync ends the session and the next time you start, you need to start all over again. Taking into account the huge sizes of volumes, byte-by-byte data consistency checking becomes impossible.
* Over time, errors accumulate and there is a threat of getting an inconsistent copy of system “A” on system “B”, which negates all efforts and costs associated with maintaining Disaster Recovery. At the same time, standard utilities do not have checking features and technical personnel will not even know about the accumulated problems with unequal content of disk arrays at the Disaster Recovery center.
* To address the above-described weaknesses, the **precizer** CLI applications was created. The program allows to identify which files differ between “A” and “B” in order to resynchronize and reconcile any differences. The program works as quickly as possible (optimized to operate close to the hardware capabilities) due to the fact that it is written in pure C and uses modern algorithms optimized for high performance. The program is designed to work with both small files and data volumes measured in petabytes and is not limited to these figures.
* The program name “**precizer**” comes from the word “precision” and means something that increases precision.
* The program traverse the contents of directories and subdirectories with high accuracy and calculates checksums for each file encountered, while storing the data in an SQLite database (a regular binary file).
* **precizer** is fault-tolerant and can continue working from the moment where it was interrupted. For example, if the program had been stopped by pressing Ctrl+C while digging a petabyte-sized file, it will NOT explore it from the beginning in the next run but will continue exactly from the point which has been already saved against the database. This saves resources and time.
* The work of this program can be interrupted at any time in any way, and this is safe both for the data being explored and for the database created by the program itself.
* In the case of a deliberate or accidental interruption of the application do not worry about the results of the failure. The result of the program's work will be completely saved and reused during subsequent runs.
* To calculate checksums, the reliable and fast SHA512 algorithm is used, which completely excludes errors even when analyzing a single petabyte-sized file's contents. If there are two thoroughly identical files of huge size, differing only by one byte, then the SHA512 algorithm will reflect this and the checksums will differ. Such result cannot be guaranteed when simpler hash functions like SHA1 or CRC32 have been used.
* The algorithms of the **precizer** app are designed in such a way that it is very easy to maintain the relevance of the data contained in the created database with paths to files and their checksums without recalculating everything from scratch. It is enough to run the program with the _--update_ parameter so that new files are added to the database, information about files erased from the disk is deleted, and for those files that have undergone modifications and their creation time or size has changed, the SHA512 checksum will be recalculated and updated in the database.
* By comparing databases from the same sources over different times, **precizer** can serve as a security monitoring tool, determining the consequences of an intrusion by identifying unauthorized modified files, whose contents may have been changed but the metadata remains the same.
* The program never changes, deletes, moves or copies any files or directories being traversed. All it does is shape lists of files and update information about them against the database. All changes occur exclusively within the boundaries of this database.
* **precizer** performance mainly depends on the performance of the disk subsystem. Each file is read byte by byte and such way a checksum is generated for each file using the SHA512 algorithm.
* The program works very quick thanks to the SQLite and FTS ([man 3 fts](https://man7.org/linux/man-pages/man3/fts.3.html)) libraries.
* Parsing of string parameters is implemented through the ARGP library
* The program is safe for cases with huge numbers of files, directories and subdirectories of any nesting. Thanks to the FTS library, recursion is not used, so there will be no stack overflow.
* Due to its compactness and portability of the codebase, the program can be used even on specialized devices such as NAS or any embedded or IoT devices.

## QUESTIONS AND BUGREPORTS

* For inquiries, access help information using _--help_ The help is made as detailed as possible to help users who do not have specialized technical knowledge.
* You can contact the author through [the github form](https://github.com/dennisrazumovsky). You can also [share a bug report there](https://github.com/dennisrazumovsky/precizer/issues/new).
* If you have questions about using the program, you can ask a question on stackoverflow using the **precizer** tag. The author is monitoring such questions and will be happy to provide his answer.

## COMPILE AND INSTALLATION

### Distribution Packaging

* The author was happy to prepare and will continue to support the compiled binary packages for Flatpak and AppImage.
* The author is NOT prepared to independently make and support future packaging of the **precizer** app for all existing OS distributions.
* If you are eager to create a package for any OS distribution and are faced with insurmountable difficulties in adapting the program code, then in this case the author will be very happy to provide all the necessary assistance in supporting the initiative and optimizing the program code for the specific distribution or package manager. How to contact the author is described in the "Questions and bug reports" section.

### Portable

Download binary-precompiled solution

#### Flatpak
TODO!

#### AppImage
TODO!

### Self-build

Nearly all utilized libraries are integrated into the program, and by default, it's built as a static executable file. This is done to increase portability and reduce dependencies. Thanks to the above, the program can be easily compiled on most modern platforms by running a few commands:


1. Install build and compile tools on Linux

#### Arch

```sh
sudo pacman -S --noconfirm base-devel cmake git unzip
```
#### Debian/Ubuntu

```sh
sudo apt -y install build-essential cmake git unzip
```

2. Get source code

```sh
git clone https://github.com/dennisrazumovsky/precizer.git
```

3. Build

```sh
cd precizer
make
```

4. Copy the resulting executable file **precizer** to any location specified in the $PATH system variable for quick access.

5. Clean everything and update

```sh

# Clean
make clean

# Update
git pull
make

# Go to point 4.
```

## EXAMPLES OF USING
To explore the program's features, you can use test sets from the tests/examples/ directory in the program source code:

### Example 1
Add files into two databases and compare them with each other:

```sh
precizer --progress --database=database1.db tests/examples/diffs/diff1

precizer --progress --database=database2.db tests/examples/diffs/diff2

precizer --compare database1.db database2.db
```
<sup>Comparison of databases database1.db and database2.db is starting...  
**These files no longer exist against database1.db but still present against database2.db**  
path1/AAA/BCB/CCC/a.txt  
path1/AAA/BCB/CCC/b.txt  
**These files no longer exist against database2.db but still present against database1.db**  
path2/AAA/ZAW/D/e/f/b_file.txt  
**The SHA512 checksums of these files do not match between database1.db and database2.db**  
1/AAA/BCB/CCC/a.txt  
2/AAA/BBB/CZC/a.txt  
3/AAA/BBB/CCC/a.txt  
4/AAA/BBB/CCC/a.txt  
path1/AAA/ZAW/D/e/f/b_file.txt  
path2/AAA/BCB/CCC/a.txt  
</sub>

### Example 2
Updating the database

Let's try to reuse previous example once agan. The first try. The warning message.

```sh
precizer --progress --database=database1.db tests/examples/diffs/diff1
```

<sub>Database file name: database1.db  
The database database1.db has been created in the past and already contains data with files and their checksums. Use the --update option if there is full confidence that update the content of the database is really need and the information about those files which was changed, removed or added should be deleted or updated against DB.  
The precizer unexpectedly ended due to an error.  
</sub>

**--update** option should be added. The _--update_ parameter is necessary to protect the database against the loss of information due to accidental executions.

```sh
precizer --update --progress --database=database1.db tests/examples/diffs/diff1
```

<sub>Database file name: database1.db  
Starting of database file database1.db integrity check...  
The database database1.db has been verified and is in good condition  
total size: 45B, total items: 58, dirs: 46, files: 12, symlnks: 0  
total size: 45B, total items: 58, dirs: 46, files: 12, symlnks: 0  
Start vacuuming...  
The database has been vacuumed  
**Nothing have been changed since the last probe (neither added nor updated or deleted files)**  
</sub>

Make some changes:

```sh
# Modify a file
echo -n "  " >> tests/examples/diffs/diff1/1/AAA/BCB/CCC/a.txt

# Add a new file
touch tests/examples/diffs/diff1/1/AAA/BCB/CCC/c.txt

# Remove a file
rm tests/examples/diffs/diff1/path2/AAA/ZAW/D/e/f/b_file.txt

```

and run the **precizer** once again:

```sh
precizer --update --progress --database=database1.db tests/examples/diffs/diff1
```

<sub>Database file name: database1.db  
Starting of database file database1.db integrity check...  
The database database1.db has been verified and is in good condition  
total size: 43B, total items: 58, dirs: 46, files: 12, symlnks: 0  
The **--update** option has been used, so the information about files will be updated against the database database1.db  
**These files have been added or changed and those changes will be reflected against the DB database1.db:**  
1/AAA/BCB/CCC/a.txt changed size & ctime & mtime  
1/AAA/BCB/CCC/c.txt adding  
total size: 43B, total items: 58, dirs: 46, files: 12, symlnks: 0  
**These files are ignored or no longer exist and will be deleted against the DB database1.db:**  
path2/AAA/ZAW/D/e/f/b_file.txt  
Start vacuuming...  
The database has been vacuumed  
</sub>

In every run of **precizer**, it traverses the file system to verify whether there is an entry about certain file in the database. In other words, the state of the file system on the disk takes priority for the program.

**precizer** works very similarly to directory traversal with rsync because it employs a similar algorithm.

Please note that **precizer** will not recalculate SHA512 checksums for files that have already been written to the database and for which the file metadata (such as creation time, modification time, and size) remains the same.

Any new files, deleted files, or files that have changed between runs of the application will be processed, and all changes will be reflected in the database if the _--update_ option is specified.

### Example 3
Using the _--silent_ mode. When this mode is enabled, the program does not display anything on the screen. This makes sense when using the program inside scripts.

Let's add the **--silent** option to the previous example:

```sh
precizer --silent --update --progress --database=database1.db tests/examples/diffs/diff1
```

As a result, nothing will be displayed on the screen

### Example 4
Additional information in _--verbose_ mode. May be useful for debugging.

Let's add the **--verbose** option to the previous example

```sh
precizer --verbose --update --progress --database=database1.db tests/examples/diffs/diff1
```
<sub>2024-03-09 22:56:49:748 src/parse_arguments.c:230:parse_arguments:Configuration: paths=tests/examples/diffs/diff1; verbose=yes; silent=no; force=no; update=yes; progress=yes; compare=no  
2024-03-09 22:56:49:748 src/init_signals.c:033:init_signals:Set signal SIGUSR2 OK:pid:462822  
2024-03-09 22:56:49:748 src/init_signals.c:045:init_signals:Set signal SIGINT OK:pid:462822  
2024-03-09 22:56:49:748 src/init_signals.c:057:init_signals:Set signal SIGTERM OK:pid:462822  
2024-03-09 22:56:49:748 src/db_init.c:045:db_init:The database has been successfully initialized  
2024-03-09 22:56:49:748 src/db_init.c:057:db_init:Opened database successfully  
2024-03-09 22:56:49:748 src/db_already_exists.c:054:db_already_exists:The database has already been created in the past  
2024-03-09 22:56:49:748 src/db_check_up_paths.c:144:db_check_up_paths:The paths written against the database and the paths passed as arguments are completely identical. Nothing will be lost  
2024-03-09 22:56:49:749 src/file_list.c:244:file_list:total size: 43B, total items: 55, dirs: 44, files: 11, symlnks: 0  
2024-03-09 22:56:49:749 src/file_list.c:244:file_list:total size: 43B, total items: 55, dirs: 44, files: 11, symlnks: 0  
2024-03-09 22:56:49:749 src/db_vacuum.c:021:db_vacuum:Start vacuuming...  
2024-03-09 22:56:49:750 src/db_vacuum.c:035:db_vacuum:The database has been vacuumed  
2024-03-09 22:56:49:750 src/status_of_changes.c:015:status_of_changes:**Nothing have been changed since the last probe (neither added nor updated or deleted files)**  
2024-03-09 22:56:49:750 src/exit_status.c:026:exit_status:The precizer completed its execution without any issues.  
2024-03-09 22:56:49:750 src/exit_status.c:027:exit_status:Enjoy your life!  
</sub>

### Example 5
Disable recursion with _--maxdepth_ option

```sh
tree tests/examples/4
tests/examples/4
├── AAA
│   ├── BBB
│   │   ├── CCC
│   │   │   └── a.txt
│   │   └── uuu.txt
│   └── tttt.txt
└── sss.txt

3 directories, 4 files
```

The _--maxdepth_ option with a value of _=0_ disables recursion completely.

```sh
precizer --maxdepth=0 tests/examples/4
```
<sub>Database file name: myhost.db  
Recursion depth limited to: 0  
**These files will be added against the DB myhost.db:**  
sss.txt  
Start vacuuming...  
The database has been vacuumed  
The precizer completed its execution without any issues.  
</sub>

### Example 6

Relative path to ignore. PCRE2 regular expressions could be used to specify a pattern to ignore files or directories. Attention! All paths in the regular expression must be specified as **relative**.

To check up and test your PCRE2 regular expressions you can use https://regex101.com/

To understand what a relative path looks like, just run directory traverses without the _--ignore_ option and look up how the terminal will display relative paths that are writing to the database:

```sh
% tree -L 3 tests/examples/diffs
tests/examples/diffs
├── diff1
│   ├── 1
│   │   └── AAA
│   ├── 2
│   │   └── AAA
│   ├── 3
│   │   └── AAA
│   ├── 4
│   │   └── AAA
│   ├── path1
│   │   └── AAA
│   └── path2
│       └── AAA
└── diff2
    ├── 1
    │   └── AAA
    ├── 2
    │   └── AAA
    ├── 3
    │   └── AAA
    ├── 4
    │   └── AAA
    ├── path1
    │   └── AAA
    └── path2
        └── AAA

26 directories, 0 files
```

```sh
precizer --ignore="diff2/1/.*" tests/examples/diffs
```

In this example the starting path for the traverses is ./tests/examples/diffs and the path to ignore will be ./tests/examples/diffs/diff2/1/ with all subdirectories (/*).

<sub>Database file name: myhost.db  
Starting of database file myhost.db integrity check...  
The database myhost.db is in good condition  
**These files will be added against the DB myhost.db:**  
diff1/3/AAA/BBB/CCC/a.txt  
diff1/path2/AAA/BCB/CCC/a.txt  
diff1/path2/AAA/ZAW/A/b/c/a_file.txt  
diff1/path2/AAA/ZAW/D/e/f/b_file.txt  
diff1/1/AAA/BCB/CCC/a.txt  
diff1/1/AAA/ZAW/A/b/c/a_file.txt  
diff1/1/AAA/ZAW/D/e/f/b_file.txt  
diff1/4/AAA/BBB/CCC/a.txt  
diff1/2/AAA/BBB/CZC/a.txt  
diff1/path1/AAA/ZAW/A/b/c/a_file.txt  
diff1/path1/AAA/ZAW/D/e/f/b_file.txt  
diff2/3/AAA/BBB/CCC/a.txt  
diff2/path2/AAA/BCB/CCC/a.txt  
diff2/path2/AAA/ZAW/A/b/c/a_file.txt  
**ignored** diff2/1/AAA/BCB/CCC/a.txt  
**ignored** diff2/1/AAA/ZAW/A/b/c/a_file.txt  
**ignored** diff2/1/AAA/ZAW/D/e/f/b_file.txt  
diff2/4/AAA/BBB/CCC/a.txt  
diff2/2/AAA/BBB/CZC/a.txt  
diff2/path1/AAA/BCB/CCC/b.txt  
diff2/path1/AAA/BCB/CCC/a.txt  
diff2/path1/AAA/ZAW/A/b/c/a_file.txt  
diff2/path1/AAA/ZAW/D/e/f/b_file.txt  
Start vacuuming...  
The database has been vacuumed  
</sub>

Let's repeat the same example but without the _--ignore_ option to add three previously ignored files:

```sh
precizer --update tests/examples/diffs
```

<sub>Database file name: myhost.db  
The database has already been created in the past  
Starting of database file myhost.db integrity check...  
The database myhost.db is in good condition  
The **--update** option has been used, so the information about files will be updated against the database myhost.db  
**These files have been added or changed and those changes will be reflected against the DB myhost.db:**  
diff2/1/AAA/BCB/CCC/a.txt adding  
diff2/1/AAA/ZAW/A/b/c/a_file.txt adding  
diff2/1/AAA/ZAW/D/e/f/b_file.txt adding  
Start vacuuming...  
The database has been vacuumed  
</sub>

### Example 7

Continuation of previous example [Example 6](#example-6).

Multiple regular expressions for ignoring can be specified using many _--ignore_ options simultaneously:

```sh
precizer --ignore="diff2/1/.*" --ignore="diff2/2/.*" tests/examples/diffs
```

### Example 8

The database will be cleared of file mentions that match the regular expressions from the arguments _--ignore:_ "diff2/1/.\*" and "diff2/2/.\*"

_--db-clean-ignored_ option must be specified additionally in order to remove from the database mention of files that matches the regular expression passed through the _--ignore_ option(s). This serves as protection against accidental deletion of information from the database.


```sh
# Let's delete the old database and create a new one, fill it out with data:

rm "${HOST}.db"

precizer tests/examples/diffs
```

<sub>Database file name: myhost.db  
Starting of database file myhost.db integrity check...  
The database myhost.db is in good condition  
**These files will be added against the DB myhost.db:**  
diff1/3/AAA/BBB/CCC/a.txt  
diff1/path2/AAA/BCB/CCC/a.txt  
diff1/path2/AAA/ZAW/A/b/c/a_file.txt  
diff1/path2/AAA/ZAW/D/e/f/b_file.txt  
diff1/1/AAA/BCB/CCC/a.txt  
diff1/1/AAA/ZAW/A/b/c/a_file.txt  
diff1/1/AAA/ZAW/D/e/f/b_file.txt  
diff1/4/AAA/BBB/CCC/a.txt  
diff1/2/AAA/BBB/CZC/a.txt  
diff1/path1/AAA/ZAW/A/b/c/a_file.txt  
diff1/path1/AAA/ZAW/D/e/f/b_file.txt  
diff2/3/AAA/BBB/CCC/a.txt  
diff2/path2/AAA/BCB/CCC/a.txt  
diff2/path2/AAA/ZAW/A/b/c/a_file.txt  
diff2/1/AAA/BCB/CCC/a.txt  
diff2/1/AAA/ZAW/A/b/c/a_file.txt  
diff2/1/AAA/ZAW/D/e/f/b_file.txt  
diff2/4/AAA/BBB/CCC/a.txt  
diff2/2/AAA/BBB/CZC/a.txt  
diff2/path1/AAA/BCB/CCC/b.txt  
diff2/path1/AAA/BCB/CCC/a.txt  
diff2/path1/AAA/ZAW/A/b/c/a_file.txt  
diff2/path1/AAA/ZAW/D/e/f/b_file.txt  
Start vacuuming...  
The database has been vacuumed  
</sub>

```sh

# Update the database by deleting information about those files which have been specified as ignored

precizer --update --db-clean-ignored \
	--ignore="diff2/1/.*" \
	--ignore="diff2/2/.*" \
	tests/examples/diffs
```
<sub>Database file name: myhost.db  
The database has already been created in the past  
Starting of database file myhost.db integrity check...  
The database myhost.db is in good condition  
**These files are ignored or no longer exist and will be deleted against the DB myhost.db:**  
clean ignored diff2/1/AAA/BCB/CCC/a.txt  
clean ignored diff2/1/AAA/ZAW/A/b/c/a_file.txt  
clean ignored diff2/1/AAA/ZAW/D/e/f/b_file.txt  
clean ignored diff2/2/AAA/BBB/CZC/a.txt  
Start vacuuming...  
The database has been vacuumed  
</sub>

### Example 9

Using the _--ignore_ option(s) together with _--include_

```sh
# Let's delete the old database and create a new one, fill it out with data:

rm "${HOST}.db"

precizer tests/examples/diffs
```

<sub>Database file name: myhost.db  
Starting of database file myhost.db integrity check...  
The database myhost.db is in good condition  
**These files will be added against the DB myhost.db:**  
diff1/3/AAA/BBB/CCC/a.txt  
diff1/path2/AAA/BCB/CCC/a.txt  
diff1/path2/AAA/ZAW/A/b/c/a_file.txt  
diff1/path2/AAA/ZAW/D/e/f/b_file.txt  
diff1/1/AAA/BCB/CCC/a.txt  
diff1/1/AAA/ZAW/A/b/c/a_file.txt  
diff1/1/AAA/ZAW/D/e/f/b_file.txt  
diff1/4/AAA/BBB/CCC/a.txt  
diff1/2/AAA/BBB/CZC/a.txt  
diff1/path1/AAA/ZAW/A/b/c/a_file.txt  
diff1/path1/AAA/ZAW/D/e/f/b_file.txt  
diff2/3/AAA/BBB/CCC/a.txt  
diff2/path2/AAA/BCB/CCC/a.txt  
diff2/path2/AAA/ZAW/A/b/c/a_file.txt  
diff2/1/AAA/BCB/CCC/a.txt  
diff2/1/AAA/ZAW/A/b/c/a_file.txt  
diff2/1/AAA/ZAW/D/e/f/b_file.txt  
diff2/4/AAA/BBB/CCC/a.txt  
diff2/2/AAA/BBB/CZC/a.txt  
diff2/path1/AAA/BCB/CCC/b.txt  
diff2/path1/AAA/BCB/CCC/a.txt  
diff2/path1/AAA/ZAW/A/b/c/a_file.txt  
diff2/path1/AAA/ZAW/D/e/f/b_file.txt  
Start vacuuming...  
The database has been vacuumed  
</sub>

PCRE2 regular expressions of relative paths to be included. Include specified relative paths even if they were previously excluded via the _--ignore_ option(s). Multiple regular expressions could be specified with --include

To check up and test your PCRE2 regular expressions you can use https://regex101.com/

The database will be cleared of file mentions that match the regular expressions from the arguments _--ignore:_ "^.\*/path2/.\*" and "diff2/.\*" but paths matching the patterns from _--include_ will remain in the database

_--db-clean-ignored_ option must be specified additionally in order to remove from the database mention of files that matches the regular expression passed through the _--ignore_ option(s). This is special protection against accidental deletion of information from the database.

```sh

# Update the database by deleting information about those files which have been specified as ignored

precizer --update --db-clean-ignored \
	--ignore="^.*/path2/.*" \
	--ignore="diff2/.*" \
	--include="diff2/1/AAA/ZAW/A/b/c/.*" \
	--include="diff2/path1/AAA/ZAW/.*" \
	tests/examples/diffs
```

<sub>Database file name: myhost.db  
Starting of database file myhost.db integrity check...  
The database myhost.db has been verified and is in good condition  
The **--update** option has been used, so the information about files will be updated against the database myhost.db  
**These files are ignored or no longer exist and will be deleted against the DB myhost.db:**  
clean ignored diff1/path2/AAA/BCB/CCC/a.txt  
clean ignored diff1/path2/AAA/ZAW/A/b/c/a_file.txt  
clean ignored diff1/path2/AAA/ZAW/D/e/f/b_file.txt  
clean ignored diff2/1/AAA/BCB/CCC/a.txt  
clean ignored diff2/1/AAA/ZAW/D/e/f/b_file.txt  
clean ignored diff2/2/AAA/BBB/CZC/a.txt  
clean ignored diff2/3/AAA/BBB/CCC/a.txt  
clean ignored diff2/4/AAA/BBB/CCC/a.txt  
clean ignored diff2/path1/AAA/BCB/CCC/a.txt  
clean ignored diff2/path1/AAA/BCB/CCC/b.txt  
clean ignored diff2/path2/AAA/BCB/CCC/a.txt  
clean ignored diff2/path2/AAA/ZAW/A/b/c/a_file.txt  
Start vacuuming...  
The database has been vacuumed  
</sub>
