MODULE = CryptX         PACKAGE = Crypt::Mode::CFB

### BEWARE - GENERATED FILE, DO NOT EDIT MANUALLY!

Crypt::Mode::CFB
_new(char * cipher_name, int rounds=0)
    CODE:
    {
        Newz(0, RETVAL, 1, struct cfb_struct);
        if (!RETVAL) croak("FATAL: Newz failed");
        RETVAL->direction = 0;
        RETVAL->cipher_rounds = rounds;
        RETVAL->cipher_id = find_cipher(cipher_name);
        if(RETVAL->cipher_id==-1) croak("FATAL: find_cipfer failed for '%s'", cipher_name);
    }
    OUTPUT:
        RETVAL

void
DESTROY(Crypt::Mode::CFB self)
    CODE:
        Safefree(self);

int
_get_dir(Crypt::Mode::CFB self)
    CODE:
        RETVAL = self->direction;
    OUTPUT:
        RETVAL

void
_start(Crypt::Mode::CFB self, int dir, SV * key, SV * iv)
    CODE:
    {
        STRLEN k_len=0;
        unsigned char *k=NULL;
        STRLEN i_len=0;
        unsigned char *i=NULL;
        int rv;

        if (!SvPOK(key))   croak("FATAL: key must be string/buffer scalar");
        k = (unsigned char *) SvPVbyte(key, k_len);

        if (!SvPOK(iv))    croak("FATAL: iv must be string/buffer scalar");
        i = (unsigned char *) SvPVbyte(iv, i_len);
        if (i_len != (STRLEN)cipher_descriptor[self->cipher_id].block_length) {
          croak ("FATAL: sizeof(iv) should be equal to blocksize (%d)", cipher_descriptor[self->cipher_id].block_length);
        }

        rv = cfb_start(self->cipher_id, i, k, (int)k_len, self->cipher_rounds, &self->state);
        if (rv != CRYPT_OK) {
          croak("FATAL: cfb_start failed: %s", error_to_string(rv));
        }

        self->direction = dir;
    }

SV *
_crypt(Crypt::Mode::CFB self, SV * data)
    CODE:
    {
        int rv;
        STRLEN in_data_len;
        unsigned char *in_data, *out_data;

        in_data = (unsigned char *)SvPVbyte(data, in_data_len);
        if (in_data_len==0) {
          RETVAL = newSVpvn("", 0);
        }
        else {
          RETVAL = NEWSV(0, in_data_len);
          SvPOK_only(RETVAL);
          SvCUR_set(RETVAL, in_data_len);
          out_data = (unsigned char *)SvPV_nolen(RETVAL);

          if (self->direction == 1) {
            rv = cfb_encrypt(in_data, out_data, (unsigned long)in_data_len, &self->state);
            if (rv != CRYPT_OK) croak("FATAL: cfb_encrypt failed: %s", error_to_string(rv));
          }
          else if (self->direction == -1) {
            rv = cfb_decrypt(in_data, out_data, (unsigned long)in_data_len, &self->state);
            if (rv != CRYPT_OK) croak("FATAL: cfb_decrypt failed: %s", error_to_string(rv));
          }
          else {
            croak("FATAL: cfb_crypt failed: call start_encrypt or start_decrypt first");
          }
        }
    }
    OUTPUT:
        RETVAL

SV *
_finish(Crypt::Mode::CFB self)
    CODE:
          self->direction = 0;
          RETVAL = newSVpvn("", 0);
    OUTPUT:
        RETVAL
