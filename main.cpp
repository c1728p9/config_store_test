#include "mbed.h"
#include "rtos.h"

#include <string.h>

#include "configuration-store/configuration_store.h"


int main()
{
    int32_t status;
    ARM_CFSTORE_DRIVER *cfstoreDriver = &cfstore_driver;
    ARM_CFSTORE_CAPABILITIES caps;

    ARM_CFSTORE_KEYDESC keyDesc1;
    ARM_CFSTORE_HANDLE_INIT(hkey1);

    ARM_CFSTORE_KEYDESC keyDesc2;
    ARM_CFSTORE_HANDLE_INIT(hkey2);

    printf("---Staring test---\n");

    // Initialize configuration store
    status = cfstoreDriver->Initialize(NULL, NULL);
    if (status < ARM_DRIVER_OK) {
        error("Initialize failed: %i\n", status);
    }

    // Get capabilities
    memset(&caps, 0, sizeof(caps));
    caps = cfstoreDriver->GetCapabilities();
    if (caps.asynchronous_ops == true) {
        error("Must be sync mode\n");
    }

    /* ---- Create a file ---- */
    printf("Creating file\n");

    // Create a new file
    memset(&keyDesc1, 0, sizeof(keyDesc1));
    keyDesc1.drl = ARM_RETENTION_NVM;
    keyDesc1.flags.read = true;
    keyDesc1.flags.write = true;
    char filename[] = "file1";
    status = cfstoreDriver->Create(filename, 1024, &keyDesc1, hkey1);
    if (status < ARM_DRIVER_OK) {
        error("Create failed: %i\n", status);
    }

    // Write data to file
    char data[] = "some test data";
    ARM_CFSTORE_SIZE writeCount = sizeof(data);
    status = cfstoreDriver->Write(hkey1, (const char *)data, &writeCount);
    if (status < ARM_DRIVER_OK) {
        error("Write failed: %i\n", status);
    }

    /* -------------------------------------------------- */
    printf("---Allocating memory to force realloc to copy pointer---\n");

    void *temp_buf = malloc(1024);
    printf("Memory allocated at %p\n", temp_buf);

    /* -------------------------------------------------- */
    printf("---Opening second file to force resize with realloc---\n");

    // Create a new file
    memset(&keyDesc2, 0, sizeof(keyDesc2));
    keyDesc2.drl = ARM_RETENTION_NVM;
    keyDesc2.flags.read = true;
    keyDesc2.flags.write = true;
    char filename2[] = "file2";
    status = cfstoreDriver->Create(filename2, 1024, &keyDesc2, hkey2);
    if (status < ARM_DRIVER_OK) {
        error("Create failed: %i\n", status);
    }

    // Write data to file
    char data2[] = "some test data";
    writeCount = sizeof(data);
    status = cfstoreDriver->Write(hkey2, (const char *)data2, &writeCount);
    if (status < ARM_DRIVER_OK) {
        error("Write failed: %i\n", status);
    }

    /* -------------------------------------------------- */
    printf("---Delete first file - operation sometimes performed on wrong pointer---\n");

    // Delete file
    status = cfstoreDriver->Delete(hkey1);
    if (status < ARM_DRIVER_OK) {
        error("Delete failed: %i\n", status);
    }

    // Close file
    status = cfstoreDriver->Close(hkey1);
    if (status < ARM_DRIVER_OK) {
        error("Close failed: %i\n", status);
    }

    /* -------------------------------------------------- */
    printf("---Call malloc to trigger a crash since heap is corrupted---\n");

    uint8_t *test_buf = (uint8_t*)malloc(1024);


    printf("---Test could not reproduce problems---\n");


}
