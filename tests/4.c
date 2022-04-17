int isdigit(char ch)
{
	return ch>='0'&&ch<='9';
}

void main()
{
	char		c;
	put_s("c=");
	c=get_c();
	put_i(isdigit(c));
}
