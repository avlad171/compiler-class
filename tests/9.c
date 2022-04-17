struct Pt{
	int x,y;
	};

struct Pt		points[20/4+5];

int		count()
{
	int		i,n;
	for(i=n=0;i<10;i=i+1){
		if(points[i].x>=0&&points[i].y>=0)n=n+1;
		}
	return n;
}

void main()
{
	put_i(count());
}
