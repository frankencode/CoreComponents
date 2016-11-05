/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

namespace cc {

template<class Source>
class SourceIterator {
public:
    SourceIterator(Source *s = 0): s_(s) { getNext(); }

    inline const typename Source::Item &operator*() const { return x_; }
    inline SourceIterator &operator++() { getNext(); return *this; }
    inline bool operator!=(const SourceIterator &b) const { return s_ != b.s_; }

private:
    inline void getNext() { if (s_) { if (!s_->read(&x_)) s_ = 0; } }
    Source *s_;
    typename Source::Item x_;
};

#define CC_DEFINE_SOURCE_BEGIN_END(ClassName) \
inline SourceIterator<ClassName> begin(ClassName *s) { return SourceIterator<ClassName>(s); } \
inline SourceIterator<ClassName> end(ClassName *) { return SourceIterator<ClassName>(); } \
inline SourceIterator<ClassName> begin(Ref<ClassName> &s) { return SourceIterator<ClassName>(s); } \
inline SourceIterator<ClassName> end(Ref<ClassName> &c) { return SourceIterator<ClassName>(); }

} // namespace cc
