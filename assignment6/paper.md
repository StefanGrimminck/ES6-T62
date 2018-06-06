# Firing interrupts and reading the ADC on the LPC3250
Stefan Grimminck & Skip Geldens
T62

## Part 1: Discovery 
We set out to make a kernel module for a customer that want to read the ADC values via a character device. First we created a program to write the ADC values to the kernel log when the button connected to EINT0 was pressed. After that we impelented the devfs_read() so that the user could make use of the converted ADC values via device nodes.

### triggering an interrupt on button EINT0
According to the sheet below the gp_interrupt function should be called when EINT0 is pressed. This function then starts the ADC, which calculates the conversion values. This is done for the tree ADC registers (so three times).

![Assignment 1 sheet](images/assignment_1_sheet.png)

First we need to find the port the interupt key is connected to. When searching trough the system scematics we came upon this picture:

![Interrupt Key](images/Interrupt_key_p2.10.png)

When tracing P2.10-EINT0 back to the corresponding LPC port we found out it was connected to GPI_01 and pin 7 on connector 3 (this was later used for connecting the probes for testing). We had found the LCP port by looking up "GPIO1" in Chapter 4 of the LPC3250 Development's kit - User's Guide "Signal Mapping to QVGA Base Board".

The intiale state of the button interupt is set to level-edged, which is in our case not useful. This is why we first set the trigger to level-edged with the following c code.
```c
	data = READ_REG(SIC2_ATR);
	data |= EINT0_LOC;
	WRITE_REG (data, SIC2_ATR);
 ``` 
 Where SIC2_ATTR is the Sub2 Activation Type Register and EINT0_LOC is the 23th bit in the register (GPI_1). See table below:
 
 ![ACtivation Type Register SIC2_ATR](https://github.com/StefanGrimminck/ES6-T62/blob/master/assignment6/images/Activation_Type_Register.png)
 
 ### Convertions with the ADC
 Now that the button works and triggers the `adc_interrupt (int irq, void * dev_id)` function, we can start working on using the ADC itself.

The ADC interrupt are requested with the following code:
```c
    if (request_irq (IRQ_LPC32XX_TS_IRQ, adc_interrupt, IRQF_DISABLED, "IRQ_ADC_INT_INTERRUPT", NULL) != 0)
    {
        printk(KERN_ALERT "ADC IRQ request failed\n");
    }
  ```
This code was already written for us, but we had to find the interrupt line to allocate ourselves, wich became 'IRQ_LPC32XX_TS_IRQ' for the adc interrupt and 'IRQ_LPC32XX_GPI_01' for the button interrupt.

The ADC interrupt line to allocated is located in "Interrupt Enable Register for Sub Interrupt Controller 1"  with bit 7 (Touch screen irq interrupt)

When the EINT0 button is pressed, the GP_Interupt will fire and call function start_adc() this function selects the right A/D channel to convert with the following code:
```c
WRITE_REG((data & ~0x0030) | ((channel << 4) & 0x0030), ADC_SELECT);
```
and start the ADC convertion with:
```c
    /* Bit 2 in register ADC_CTRL set => the ADC is powered up and reset */
   	data = READ_REG(ADC_CTRL);
	data |= AD_PDN_CTRL;
	WRITE_REG (data, ADC_CTRL);
```
This code has been created according to the A/D Control Register:
![A/D Control Register](https://github.com/StefanGrimminck/ES6-T62/blob/master/assignment6/images/AD_control_register.png)

After the conversion has finished the the `adc_interrupt (int irq, void * dev_id)` is called and values logged into the kernel log.

Both the interrupts can be found in /proc/interrupts `IRQ_ADC_INT_INTERRUPT` for our ADC and `IRQ_GPI_01_INTERRUPT` for our button:
![interrupts](https://github.com/StefanGrimminck/ES6-T62/blob/master/assignment6/images/adc_interrupts.png)

