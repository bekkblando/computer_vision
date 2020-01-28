cpsc482 - Section 1
Bekk Blando
Gives Sobel Transform dx, dy
Lessons Learned: My program didn't work for a while because I read things in as a block of unsigned chars.
I knew I needed to maintain more accuracy so I switched to reading in as a block of doubles. The program only started working once
I read the data as unsigned chars and casted to doubles.

Run using make; ./main mandrill.pgm