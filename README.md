# Registration form

## Server:

### Technologies:

- C++ programming language.
- Winsock - used for network programming and managment of network sockets.
- Windows Sockets 2(WS2) - extension of Winsock, providing additional features.
- SQL Connectivity (ODBC) - Library used to connect and interact with a database.
- cURL - a library used for transfering data using various protocols.

### Functionalities:

- Data validation - the User class is initiated with username, email and password. During the init method, two more methods are called, one of them verifies the username by checking if the size is less than 10 symbols if there are invalid symbols in the username. The other method verifies the email by checking if it is in the correct format- {username}@{email_provider}.{domain}
- Saving the data in a relational database - the function registrationSuccess first checks if the given username is taken, if not, it inserts the new user into the database.
- Sending a verification email - the function sendEmail takes the email of the new user and sends a confirmation email, using cURL functionalities, thanking the user for registering. (There has been some technical issues with the function not being able to log in the sender's account, so it hasn't been tested.)
- login/logout - depending on the information, sent by the client, the server has a login functionality. The function loginSuccess sends a query to the SQL server and checks if the given username and password are in the database. If they are, a confirmation is sent to the client, if not, a denial is sent. No logout functionality.
  
### Ready- made functions:
- Every function from the curl.h, sql.h, sqlext.h, sqltypes.h, winsock2.h.
- wcsmp(...) - comparing two wide strings (wstring) or wchar_t*.
- mbstowcs_s(...) - a function used to convert const char* to wchar_t*.
- strlen(...) - a function used to get the size of a given const char*.

### Files:
- User.h - a header file, used to declare the User class, and its methods
- User.cpp - a cpp file, used to implement the User class, which stores and validates the data of a single user.
- Server.cpp - a cpp file, containing the main function. The Server.cpp file is used for connection with the database, communication between the server and the client and some additional functionalities, connected to the use of the database.


## Client:

### Technologies:
- C++ programming language.
- ImGui(Dear ImGui) - used for creating graphical user interfaces in applications.
- Windows Api -  for handling various system-level functions and operations in a Windows environment.
- OpenGL and GLFW - used for handling graphics rendering and window management in an OpenGL context.
- GLAD - used for managing OpenGL function pointers.
- Winsock - used for network programming in a Windows environment.
- tchar.h - used to handle character set conversions.

### Functionalities:

- CRUD interface - Using imGui, openGL and GLFW. The beginning screen is a login screen with two boxes (for username and password), an "enter" button and a "register" button. Clicking the "enter" button sends the information to the server for validation. If the user exists and the password is correct, a new window is showed, saying "Login success!". By clicking the "register" button, a new screen is showed- this time a registration form, with three boxes - username, password and email, "back" button and "enter" button. After entering the username, password and email and clicking the "Enter" button, the information is sent to the server. If the registration has been successful, a new window is showed with confirmation, otherwise the window says "unsuccessful registration". By clicking the "back" button, the user is returned to the login page.

### Ready- made functions:
- Every function from the imgui.h, Windows.h, glad.h, glfw3.h, winsock2.h, ws2tcpip.h.

### Files:
- UIHandle.h - a header file, used to declare the UIHandle class and its methods.
- UIHandle.cpp - a cpp file, used to implement the UIHandle class.   
  Init(...) method - takes width and height of a window and initializes the glfw window and the imGui context and main colors.  
  run() method - the main loop of the UI. Calls the newFrame, update and render methods in a loop, until the window is closed, or the update method returns false.  
  newFrame() method - uses ImGui::newFrame to refresh the current frame.  
  bool update() method - the main logic is here. Checks if the login or the register window should be open. The rest is explained under "Functionalities".  
  render() method - uses ImGui::Render(), which renders the new frame with the updated data.  
  getInfo - returns the information vector, which contains the username, password and email.  
  titleText(title) method - displays a given title in a specific location.  
  userPassInput() - displays the input boxes of the username password and the email. the password is censored with "*".  
  result() method - creates the resulting window with a set size and a given text. ("Login success!")
- Client.cpp - a cpp file, containing the main function. Used for comunication with the server.
