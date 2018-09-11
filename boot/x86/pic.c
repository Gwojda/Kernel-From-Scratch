#include "pic.h"
#include "io.h"

static void	pic_initialize_parameter(
		u32 commande_addr, u32 data_addr,
		u8 icw1, u8 icw2, u8 icw3, u8 icw4)
{
	outb(commande_addr, icw1);
	outb(data_addr, icw2);
	outb(data_addr, icw3);
	if (icw1 & 0b00000001)
		outb(data_addr, icw4);
}

static void	pic_end_of_interupt_with_slave(int is_on_slave)
{
	if (is_on_slave)
		outb(PIC_SLAVE_COMMANDE, PIC_OCW2_NORMAL_EOI);
	outb(PIC_MASTER_COMMANDE, PIC_OCW2_NORMAL_EOI);
}

static void	pic_interupt_mask_addr(u32 data_addr, u8 mask)
{
	outb(data_addr, mask);
}

void		pic_initialize(void)
{
	pic_initialize_parameter(PIC_MASTER_COMMANDE, PIC_MASTER_DATA,
			PIC_ICW1_ICW4 | PIC_ICW1_REQUEST, 0x20, PIC_ICW3_SLAVE_ON_2, PIC_ICW4_X86);
	pic_initialize_parameter(PIC_SLAVE_COMMANDE, PIC_SLAVE_DATA,
			PIC_ICW1_ICW4 | PIC_ICW1_REQUEST, 0x28, PIC_ICW3_MASTER_ON_2, PIC_ICW4_X86);
}

void		pic_interupt_mask(u16 mask)
{
	pic_interupt_mask_addr(PIC_MASTER_DATA, mask & 0b11111111);
	pic_interupt_mask_addr(PIC_SLAVE_DATA, mask >> 8);
}

void		pic_end_of_interupt(u8 int_type)
{
	if (int_type < PIC_INT_BASE
			|| int_type >= 8 * 2 + PIC_INT_BASE)
		// out of pic range
		return ;
	else if (int_type >= 8 + PIC_INT_BASE)
		pic_end_of_interupt_with_slave(1);
	else
		pic_end_of_interupt_with_slave(0);
}
