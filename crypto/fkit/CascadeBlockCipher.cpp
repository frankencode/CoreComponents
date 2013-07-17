#include "CascadeBlockCipher.h"

namespace fkit
{

CascadeBlockCipher::CascadeBlockCipher(BlockCipher *cipher, ByteArray *iv)
	: BlockCipher(cipher->blockSize()),
	  cipher_(cipher),
	  s_(iv->copy(0, cipher->blockSize()))
{}

void CascadeBlockCipher::encode(ByteArray *p, ByteArray *c)
{
	*s_ ^= *p;
	cipher_->encode(s_, c);
	*s_ = *c;
}

void CascadeBlockCipher::decode(ByteArray *c, ByteArray *p)
{
	cipher_->decode(c, p);
	*p ^= *s_;
	*s_ = *c;
}

} // namespaec fkit
