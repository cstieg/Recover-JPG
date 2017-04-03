/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Recovers JPEGs from a forensic image.
 */

 #include <stdlib.h>
 #include <stdio.h>
 #include <string.h>

int main(int argc, char* argv[])
{
    FILE* input_file = fopen("card.raw", "rb");
    if (input_file == NULL)
    {
        printf("File not found!");
        return 1;
    }

    FILE* output;

    unsigned char fat_block[512];
    unsigned char JPG_key1[4] = {0xff, 0xd8, 0xff, 0xe0};
    unsigned char JPG_key2[4] = {0xff, 0xd8, 0xff, 0xe1};
    size_t read_result;

    // discard initial blocks that don't begin with the key
    do
    {
        read_result = fread(&fat_block, 512, 1, input_file);
    }
    while (read_result == 1 && memcmp(&fat_block, JPG_key1, 4) != 0 && memcmp(&fat_block, JPG_key2, 4) != 0);

    if (read_result != 1)
    {
        printf("No matching data found!");
        return 2;
    }

    for (int i = 0; read_result == 1; i++)
    {
        // Open output file
        char file_name[10];
        sprintf(file_name, "%03d.jpg", i);
        output = fopen(file_name, "wb");

        // write initial block that begins with the key
        // if there is any matching data, it will already be in the buffer from the do while above
        fwrite(&fat_block, 512, 1, output);
        read_result = fread(&fat_block, 512, 1, input_file);

        // write any further blocks that do not begin with the key, part of the same file
        while (read_result == 1 && memcmp(&fat_block, JPG_key1, 4) != 0 && memcmp(&fat_block, JPG_key2, 4) != 0)
        {
            fwrite(&fat_block, 512, 1, output);
            read_result = fread(&fat_block, 512, 1, input_file);
        }

        // if the key is found, close file, loop to next iteration, unless read_result == 1, in which case for loop will end
        fclose(output);
    }
    fclose(input_file);
    return 0;
}
