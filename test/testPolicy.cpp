template<class T, template<class> class P>
class C: public P< C<T,P> >
{
public:
};

template<class N>
class M {};

int main()
{
	C<int, M> c;
	return 0;
}
