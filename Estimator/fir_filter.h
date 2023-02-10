double sinc(double x)
{
  double y;
  
  if (x == 0.0)
  {
    y = 1.0;
  }
  else
  {
    y = sin(x) / x;
  }
  
  return y;
}

void Hanning_window(int16_t w[], int N)
{
  int n;
  
  if (N % 2 == 0) /* Nが偶数のとき */
  {
    for (n = 0; n < N; n++)
    {
      w[n] = 0.5 - 0.5 * cos(2.0 * M_PI * n / N);
    }
  }
  else /* Nが奇数のとき */
  {
    for (n = 0; n < N; n++)
    {
      w[n] = 0.5 - 0.5 * cos(2.0 * M_PI * (n + 0.5) / N);
    }
  }
}


void FIR_LPF(double fe, int J, int16_t b[], int16_t w[])
{
  int m;
  int offset;

  offset = J / 2;
  for (m = -J / 2; m <= J / 2; m++)
  {
    b[offset + m] = 2.0 * fe * sinc(2.0 * M_PI * fe * m);
  }
  
  for (m = 0; m < J + 1; m++)
  {
    b[m] *= w[m];
  }
}

void FIR_HPF(double fe, int J, double b[], double w[])
{
  int m;
  int offset;
  
  offset = J / 2;
  for (m = -J / 2; m <= J / 2; m++)
  {
    b[offset + m] = sinc(M_PI * m) - 2.0 * fe * sinc(2.0 * M_PI * fe * m);
  }
  
  for (m = 0; m < J + 1; m++)
  {
    b[m] *= w[m];
  }
}

void FIR_BPF(double fe1, double fe2, int J, double b[], double w[])
{
  int m;
  int offset;
  
  offset = J / 2;
  for (m = -J / 2; m <= J / 2; m++)
  {
    b[offset + m] = 2.0 * fe2 * sinc(2.0 * M_PI * fe2 * m)
                  - 2.0 * fe1 * sinc(2.0 * M_PI * fe1 * m);
  }
  
  for (m = 0; m < J + 1; m++)
  {
    b[m] *= w[m];
  }
}

void FIR_BEF(double fe1, double fe2, int J, double b[], double w[])
{
  int m;
  int offset;
  
  offset = J / 2;
  for (m = -J / 2; m <= J / 2; m++)
  {
    b[offset + m] = sinc(M_PI * m)
                  - 2.0 * fe2 * sinc(2.0 * M_PI * fe2 * m)
                  + 2.0 * fe1 * sinc(2.0 * M_PI * fe1 * m);
  }
  
  for (m = 0; m < J + 1; m++)
  {
    b[m] *= w[m];
  }
}