# Client-Server-Communication
Develop two applications (named "client" and "server") which communicate between them using a protocol having the following specifications:

- the communication is made by executing commands read from the keyboard in the client app and executed in child processes in the server app;
- the commands are strings bounded by a new line;
- the responses are series of bytes prefixed by the length of the response;
- the result obtained from the execution of any command will be displayed on screen by the client app;
- the child processes in the server app do not communicate directly with the client app, only with the parent process;
- the minimal protocol includes the following commands:
    - "login : username" - whose existence is validated by using a configuration file, which contains all the users that have access to the functionalities. This command's execution will be made in a child process in the server app;
    - "get-logged-users" - displays information (username, hostname for remote login, time entry was made) about all users that are logged in the operating system (see "man 5 utmp" and "man 3 getutent"). This command should not execute if the user is not authenticated in the application. This command's execution will be made in a child process in the server app;
    - "get-proc-info : pid" - displays information (name, state, ppid, uid, vmsize) about the process whose pid is specified (source of information: the file /proc/<pid>/status). This command should not execute if the user is not authenticated in the application. This command's execution will be made in a child process in the server app;
    - "logout";
    - "quit".
- no function in the "exec()" family (or other similar, like popen(), system()...) will be used to execute some system commands that offer the same functionalities;
- the communication among processes will be done using all of the following communication mechanisms: pipes, fifos, and socketpairs.
