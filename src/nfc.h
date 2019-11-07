#pragma once
#include <NfcTag.h>

void nfc_setup(void);
boolean nfc_tag_present(void);
void nfc_read(NfcTag* tag);
