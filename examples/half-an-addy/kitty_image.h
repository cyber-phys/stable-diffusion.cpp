#ifndef KITTY_IMAGE_H
#define KITTY_IMAGE_H

#include <string>

// Function to display the image in the terminal using kitty protocol
void display_image_with_kitty_protocol(const std::string& file_path, unsigned int width, unsigned int height);

#endif // KITTY_IMAGE_H
