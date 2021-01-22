#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <math.h>

#ifndef PI
#define PI 3.14159265358979323846264338327950288
#endif

#define PPG_DEV_NAME "/dev/ppg"
#define MAX_NUM_OF_SAMPLES 2048
#define MIN_IDX ((0.5 * MAX_NUM_OF_SAMPLES) / 50) // position in the PSD of the spectral line corresponding to 30 bpm
#define MAX_IDX ((3 * MAX_NUM_OF_SAMPLES) / 50)	  // position in the PSD of the spectral line corresponding to 180 bpm

#define PEAK_TO_BPM(peak) ((peak) * 60 * 50 / MAX_NUM_OF_SAMPLES)

#define FREQ_HZ 50
#define SLEEP_US (1000000/FREQ_HZ)

typedef float real;

typedef struct
{
	real Re;
	real Im;
} complex;

static int g_keepalive = 1;

void on_signal(int signum);
void fft(complex *v, int n, complex *tmp);
void compute_psd(complex *v, int n, float *p_abs);
int get_peak(float* abs, int min, int max);

int main(int argc, char **argv)
{
	int fd = open(PPG_DEV_NAME, O_RDONLY);
	int sample;
	int num_of_samples;
	complex samples[MAX_NUM_OF_SAMPLES];
	complex tmp_vector[MAX_NUM_OF_SAMPLES];
	float abs[MAX_NUM_OF_SAMPLES];
	int m;
	int k;
	int bpm;
	ssize_t sz;

	if (fd >= 0)
	{
		num_of_samples = 0;

		signal(SIGINT, on_signal);
		signal(SIGTERM, on_signal);
		while (g_keepalive == 1)
		{
			sz = read(fd, &sample, sizeof(int));
			samples[num_of_samples].Re = (real)sample;
			samples[num_of_samples].Im = 0;
			++num_of_samples;
			if (num_of_samples >= MAX_NUM_OF_SAMPLES)
			{
				fft(samples, MAX_NUM_OF_SAMPLES, tmp_vector);
				compute_psd(samples, MAX_NUM_OF_SAMPLES, abs);
				m = get_peak(abs, MIN_IDX, MAX_IDX);
				bpm = PEAK_TO_BPM(m);
				printf("%d bpm\n", bpm);
				num_of_samples = 0;
			}

			usleep(SLEEP_US);
		}
		close(fd);
	}
	else
	{
		fprintf(stderr, "Can't open device %s\n", PPG_DEV_NAME);
		return errno;
	}
	return 0;
}

void on_signal(int signum)
{
	g_keepalive = 0;
}

void fft(complex *v, int n, complex *tmp)
{
	if (n > 1)
	{ /* otherwise, do nothing and return */
		int k, m;
		complex z, w, *vo, *ve;
		ve = tmp;
		vo = tmp + n / 2;
		for (k = 0; k < n / 2; k++)
		{
			ve[k] = v[2 * k];
			vo[k] = v[2 * k + 1];
		}
		fft(ve, n / 2, v); /* FFT on even-indexed elements of v[] */
		fft(vo, n / 2, v); /* FFT on odd-indexed elements of v[] */
		for (m = 0; m < n / 2; m++)
		{
			w.Re =  cos((double)(2 * PI * m / (double)n));
			w.Im = -sin((double)(2 * PI * m / (double)n));
			z.Re = w.Re * vo[m].Re - w.Im * vo[m].Im; /* Re(w*vo[m]) */
			z.Im = w.Re * vo[m].Im + w.Im * vo[m].Re; /* Im(w*vo[m]) */
			v[m].Re = ve[m].Re + z.Re;
			v[m].Im = ve[m].Im + z.Im;
			v[m + n / 2].Re = ve[m].Re - z.Re;
			v[m + n / 2].Im = ve[m].Im - z.Im;
		}
	}
	return;
}

void compute_psd(complex *v, int n, float *p_abs)
{
	int i;
	for (i = 0; i < MAX_NUM_OF_SAMPLES; ++i)
	{
		p_abs[i] = (50.0 / MAX_NUM_OF_SAMPLES) * ((v[i].Re * v[i].Re) + (v[i].Im * v[i].Im));
	}
}

int get_peak(float* abs, int min, int max)
{
	int peak = MIN_IDX;
	int i;
	for (i = min; i < max; ++i)
	{
		if (abs[i] > abs[peak])
		{
			peak = i;
		}
	}
	return peak;
}
