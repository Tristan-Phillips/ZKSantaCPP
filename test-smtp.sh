#!/bin/bash

# SMTP server configuration
SMTP_SERVER="your.smtp.server"
SMTP_PORT=465 # Use 587 for STARTTLS
SMTP_USER="your_username"
SMTP_PASS="your_password"
TO_EMAIL="recipient@example.com"
FROM_EMAIL="sender@example.com"
SUBJECT="Test Email from SMTP Script"
BODY="This is a test email sent using a shell script with OpenSSL."

# Encode username and password in base64
BASE64_USERNAME=$(echo -ne "\0$SMTP_USER\0$SMTP_PASS" | base64)

# Command sequence for sending an email
(
  echo "EHLO $SMTP_SERVER"
  sleep 1
  echo "AUTH PLAIN $BASE64_USERNAME"
  sleep 1
  echo "MAIL FROM:<$FROM_EMAIL>"
  sleep 1
  echo "RCPT TO:<$TO_EMAIL>"
  sleep 1
  echo "DATA"
  sleep 1
  echo -e "Subject: $SUBJECT\nFrom: $FROM_EMAIL\nTo: $TO_EMAIL\n\n$BODY\n."
  sleep 1
  echo "QUIT"
) | openssl s_client -quiet -connect $SMTP_SERVER:$SMTP_PORT -crlf

# Note: Adjust sleep times if necessary based on your server's response speed.