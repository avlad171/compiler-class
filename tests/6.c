void main()
{
	int		i,n,t;
	int		v[100];
	put_s("n=");
	n=get_i();
	for(i=0;i<n;i=i+1){
		v[i]=get_i();
		}
	for(i=0;i<n/2;i=i+1){
		t=v[i];
		v[i]=v[n-i-1];
		v[n-i-1]=t;
		}
	for(i=0;i<n;i=i+1){
		put_c('#');
		put_i(v[i]);
		}
}
