/*
 * Port-based I/O routines.  These are in a separate module because most
 * drivers will not use them, and system services are statically linked.
 */
#include <minix/drivers.h>
#include <minix/netdriver.h>
#include <assert.h>

#include "netdriver.h"

#if defined(__riscv64)
#define NETDRIVER_PORTIO_BUFSIZE 256

static int
netdriver_safe_insb(long port, endpoint_t endpt, cp_grant_id_t grant,
	size_t off, size_t count)
{
	uint8_t buf[NETDRIVER_PORTIO_BUFSIZE];
	size_t done = 0;
	int r;

	while (done < count) {
		size_t chunk = count - done;
		if (chunk > sizeof(buf))
			chunk = sizeof(buf);

		if ((r = sys_insb((u32_t)port, endpt, buf,
		    (int)chunk)) != OK)
			return r;

		if ((r = sys_safecopyto(endpt, grant, off + done,
		    (vir_bytes)buf, chunk)) != OK)
			return r;

		done += chunk;
	}

	return OK;
}

static int
netdriver_safe_outsb(long port, endpoint_t endpt, cp_grant_id_t grant,
	size_t off, size_t count)
{
	uint8_t buf[NETDRIVER_PORTIO_BUFSIZE];
	size_t done = 0;
	int r;

	while (done < count) {
		size_t chunk = count - done;
		if (chunk > sizeof(buf))
			chunk = sizeof(buf);

		if ((r = sys_safecopyfrom(endpt, grant, off + done,
		    (vir_bytes)buf, chunk)) != OK)
			return r;

		if ((r = sys_outsb((u32_t)port, endpt, buf,
		    (int)chunk)) != OK)
			return r;

		done += chunk;
	}

	return OK;
}

static int
netdriver_safe_insw(long port, endpoint_t endpt, cp_grant_id_t grant,
	size_t off, size_t count)
{
	uint16_t buf[NETDRIVER_PORTIO_BUFSIZE / sizeof(uint16_t)];
	size_t done = 0;
	int r;

	while (done < count) {
		size_t chunk = count - done;
		if (chunk > sizeof(buf) * sizeof(buf[0]))
			chunk = sizeof(buf) * sizeof(buf[0]);

		if ((r = sys_insw((u32_t)port, endpt, buf,
		    (int)(chunk / 2))) != OK)
			return r;

		if ((r = sys_safecopyto(endpt, grant, off + done,
		    (vir_bytes)buf, chunk)) != OK)
			return r;

		done += chunk;
	}

	return OK;
}

static int
netdriver_safe_outsw(long port, endpoint_t endpt, cp_grant_id_t grant,
	size_t off, size_t count)
{
	uint16_t buf[NETDRIVER_PORTIO_BUFSIZE / sizeof(uint16_t)];
	size_t done = 0;
	int r;

	while (done < count) {
		size_t chunk = count - done;
		if (chunk > sizeof(buf) * sizeof(buf[0]))
			chunk = sizeof(buf) * sizeof(buf[0]);

		if ((r = sys_safecopyfrom(endpt, grant, off + done,
		    (vir_bytes)buf, chunk)) != OK)
			return r;

		if ((r = sys_outsw((u32_t)port, endpt, buf,
		    (int)(chunk / 2))) != OK)
			return r;

		done += chunk;
	}

	return OK;
}
#endif /* defined(__riscv64) */

/*
 * Port-based I/O byte sequence copy routine.
 */
static void
netdriver_portb(struct netdriver_data * data, size_t off, long port,
	size_t size, int portin)
{
	size_t chunk;
	unsigned int i;
	int r;
#if !defined(__riscv64)
	int req;
#endif

	off = netdriver_prepare_copy(data, off, size, &i);

#if !defined(__riscv64)
	req = portin ? DIO_SAFE_INPUT_BYTE : DIO_SAFE_OUTPUT_BYTE;
#endif

	while (size > 0) {
		chunk = data->iovec[i].iov_size - off;
		if (chunk > size)
			chunk = size;
		assert(chunk > 0);

#if defined(__riscv64)
		if (portin)
			r = netdriver_safe_insb(port, data->endpt,
			    data->iovec[i].iov_grant, off, chunk);
		else
			r = netdriver_safe_outsb(port, data->endpt,
			    data->iovec[i].iov_grant, off, chunk);
#else
		r = sys_sdevio(req, port, data->endpt,
		    (void *)data->iovec[i].iov_grant, chunk, off);
#endif
		if (r != OK)
			panic("netdriver: port I/O failed: %d", r);

		i++;
		off = 0;
		size -= chunk;
	}
}

/*
 * Transfer bytes from hardware to a destination buffer using port-based I/O.
 */
void
netdriver_portinb(struct netdriver_data * data, size_t off, long port,
	size_t size)
{

	return netdriver_portb(data, off, port, size, TRUE /*portin*/);
}

/*
 * Transfer bytes from a source buffer to hardware using port-based I/O.
 */
void
netdriver_portoutb(struct netdriver_data * data, size_t off, long port,
	size_t size)
{

	return netdriver_portb(data, off, port, size, FALSE /*portin*/);
}

/*
 * Transfer words from hardware to a destination buffer using port-based I/O.
 */
void
netdriver_portinw(struct netdriver_data * data, size_t off, long port,
	size_t size)
{
	uint16_t buf;
	uint32_t value;
	size_t chunk;
	unsigned int i;
	int r, odd_byte;

	off = netdriver_prepare_copy(data, off, size, &i);

	odd_byte = 0;
	while (size > 0) {
		chunk = data->iovec[i].iov_size - off;
		if (chunk > size)
			chunk = size;
		assert(chunk > 0);

		if (odd_byte) {
			if ((r = sys_safecopyto(data->endpt,
			    data->iovec[i].iov_grant, off,
			    (vir_bytes)&((char *)&buf)[1], 1)) != OK)
				panic("netdriver: unable to copy data: %d", r);

			off++;
			size--;
			chunk--;
		}

		odd_byte = chunk & 1;
		chunk -= odd_byte;

		if (chunk > 0) {
#if defined(__riscv64)
			r = netdriver_safe_insw(port, data->endpt,
			    data->iovec[i].iov_grant, off, chunk);
#else
			r = sys_safe_insw(port, data->endpt,
			    data->iovec[i].iov_grant, off, chunk);
#endif
			if (r != OK)
				panic("netdriver: port input failed: %d", r);

			off += chunk;
			size -= chunk;
		}

		if (odd_byte) {
			if ((r = sys_inw(port, &value)) != OK)
				panic("netdriver: port input failed: %d", r);
			buf = (uint16_t)value;

			if ((r = sys_safecopyto(data->endpt,
			    data->iovec[i].iov_grant, off,
			    (vir_bytes)&((char *)&buf)[0], 1)) != OK)
				panic("netdriver: unable to copy data: %d", r);

			size--;
		}

		i++;
		off = 0;
	}
}

/*
 * Transfer words from a source buffer to hardware using port-based I/O.
 */
void
netdriver_portoutw(struct netdriver_data * data, size_t off, long port,
	size_t size)
{
	uint16_t buf;
	size_t chunk;
	unsigned int i;
	int r, odd_byte;

	off = netdriver_prepare_copy(data, off, size, &i);

	odd_byte = 0;
	while (size > 0) {
		chunk = data->iovec[i].iov_size - off;
		if (chunk > size)
			chunk = size;
		assert(chunk > 0);

		if (odd_byte) {
			if ((r = sys_safecopyfrom(data->endpt,
			    data->iovec[i].iov_grant, off,
			    (vir_bytes)&((char *)&buf)[1], 1)) != OK)
				panic("netdriver: unable to copy data: %d", r);

			if ((r = sys_outw(port, buf)) != OK)
				panic("netdriver: port output failed: %d", r);

			off++;
			size--;
			chunk--;
		}

		odd_byte = chunk & 1;
		chunk -= odd_byte;

		if (chunk > 0) {
#if defined(__riscv64)
			r = netdriver_safe_outsw(port, data->endpt,
			    data->iovec[i].iov_grant, off, chunk);
#else
			r = sys_safe_outsw(port, data->endpt,
			    data->iovec[i].iov_grant, off, chunk);
#endif
			if (r != OK)
				panic("netdriver: port output failed: %d", r);

			off += chunk;
			size -= chunk;
		}

		if (odd_byte) {
			if ((r = sys_safecopyfrom(data->endpt,
			    data->iovec[i].iov_grant, off,
			    (vir_bytes)&((char *)&buf)[0], 1)) != OK)
				panic("netdriver: unable to copy data: %d", r);

			size--;
		}

		i++;
		off = 0;
	}

	if (odd_byte) {
		((char *)&buf)[1] = 0;

		if ((r = sys_outw(port, buf)) != OK)
			panic("netdriver: port output failed: %d", r);
	}
}
