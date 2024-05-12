


float TriPrismSDF( vec3 p, vec2 h )
{
  vec3 q = abs(p);
  return max(q.z-h.y,max(q.x*0.866025+p.y*0.5,-p.y)-h.x*0.5);
}


float SmoothSubtraction( float d1, float d2, float smoothness )
{
    float h = clamp( 0.5 - 0.5*(d2+d1)/smoothness, 0.0, 1.0 );
    return mix( d2, -d1, h ) + smoothness*h*(1.0-h);
}

float SmoothIntersection( float d1, float d2, float smoothness )
{
    float h = clamp( 0.5 - 0.5*(d2-d1)/smoothness, 0.0, 1.0 );
    return mix( d2, d1, h ) + smoothness*h*(1.0-h);
}

float SDFXor(float d1, float d2 )
{
    return max(min(d1,d2),-max(d1,d2));
}