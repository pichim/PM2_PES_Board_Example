# State machine - WS3

My idea for this:
State machine with 5 states:
- initial - enable hardware
- forward - go forward - just turn motors one direction, US sensor distance less than something - go backwards
- Backward - go backward - just turn motors in oposite direction, after going backward for number of iteration go to sleep
- Sleep - just sleep state - can switch to forward by USER button (???)
- Emergency stop - if anytime mechanical button would be clicked - just stop