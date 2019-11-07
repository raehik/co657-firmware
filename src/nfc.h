#pragma once

#include <NfcTag.h>

void nfc_setup(void);
boolean nfc_tag_present(void);
void nfc_tag_read(NfcTag* tag);
