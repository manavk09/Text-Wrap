Name: Kritik Patel
NetID: ksp127
Name: Manav Kumar
NetID: mk1745

We used various files with different scenarios and types of test cases to test our program.
Some such files and scenarios contained:
- An equal number of characters to the buffer size and/or width
- Many arbitrary spaces and/or new lines between words
- Changing the buffer size to various values including 1
- Having all text on one line
- Tested very large files such as the Bee Movie Script

To test that our program runs as expected for directory inputs, we tested for:
- Directories within the directory of the project files
- Directory of the project files themself using different pathnames such as "." and "../(directory)"
- Absolute files paths (/ilab/users/(...))
- The directory prior to the current directory using ".."

In order to check for any memory leaks and other bugs within the prorgam, we ran the VALGRIND memory error detector.
