void main()
{
	int		i,n;
	double	s;
	s=0.0;
	put_s("n=");
	n=get_i();
	for(i=0;i<n;i=i+1){
		s=s+get_i();
		}
	put_s("media=");
	put_d(s/n);
}
