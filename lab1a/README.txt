my part 1 have clean console to char-at-a-time,, no-echo mode
one character at time and can handle long input
forked a process for shell
I made one thread pass keyboard to the shell
I use my parent process as another thread input from shell pipe and echoes to stdout
my ^d sends SIGHUP to the shell exit with 0 can check with echo $?
my ^C send signt to shell
(I detect sigpipe using with out eof check otherwise eof will come first)SIGPIPE from shell to parent with handle and exit with 1 can check with echo $?
EOF from shell:exit with 1 
Using waitpid report  
interprets my <lf> and <cr> as required
Restores the terminal modes to older mode everytime.