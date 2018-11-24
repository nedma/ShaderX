// Barnsley 1
float2 i;
if(z.x>=0){
	i=float2(-1,0);
}
else{
	i=float2(1,0);
}
 z=m_c_mul((m_c_sin(z+i)),Seed);
