Producer Consumer Problem:-
============================

Given the size of a circular buffer and number of readers, write a code with
n+1 threads, n for readers and 1 for the writers.All are accessing the same
circular buffer. <br>
The following conditions should be satisfied:<br>
■ A reader cannot read in an empty location.<br>
■ A reader will not read from the same location twice if the writer has not
overwritten.<br>
■ A writer cannot overwrite a place which has not been read by every
reader.<br>
**Input** : Size of buffer, n. You can use random function for writing data into the
buffer.
