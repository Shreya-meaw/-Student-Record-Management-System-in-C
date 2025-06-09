# -Student-Record-Management-System-in-C
Student Record Management System (C Language Project) — Key Points
Purpose:
Terminal-based application to manage student academic records with secure login and performance analysis.

<b>User Roles:</b>
Admin: Manage student data (add, update, delete, search).
Student: View personal report card and compare with class topper.

Authentication:
Secure login for both Admin and Students.
Password input masked for privacy.
Password reset option available after login.

Admin Features:
Add, update, delete student records.
Search students by Name, ID, or Class.
View all report cards and rankings.
Performance analytics: subject averages, top scorers.
ASCII bar chart visualizations of results.
Export reports to text files.
Change admin password and logout.

Student Features:
View marks, total, average, and rank.
See ASCII bar chart of own performance.
Compare performance with class topper visually.
Change password and logout.
Data Storage:
Student records stored in binary files (students.dat).
Admin credentials stored separately (admin.dat).
Password changes overwrite only the password field.
Analytics & Visualization:
Automatic rank calculation using sorting algorithms.
Identify subject-wise toppers and class averages.
Display performance as ASCII bar charts.

Input Validation:
Unique student IDs enforced.
Marks restricted to 0–100 range.
String length checks for names, passwords, classes.
Login verifies ID and password match.

<b>(Summary Version)</b>
Student Record Management System (C) – Designed and built a role-based academic record system with login authentication, file-based data persistence, password masking, and automated analytics including rank calculation, subject averages, and top performer charts. Integrated ASCII bar charts for visual reporting and admin/student dashboards for full control and access.
