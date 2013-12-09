#include <virtualsoc/cluster/cl_acc.h>


#define INPUT_ADDR  ACCELERATOR_MEM_ADDR
#define OUTPUT_ADDR ACCELERATOR_MEM_ADDR + sizeof(int)

//Get_word_size
uint32_t cl_acc::get_word_size( uint32_t bw )
{
  uint32_t size;

  //Word, half word or byte?
  switch (bw)
  {
    case 0 : size = 0x4; break;
    case 1 : size = 0x1; break;
    case 2 : size = 0x2; break;
    default :
      cout << "Invalid word size" << endl;
      exit(1);
  }

  return size;
}

//Execute
void cl_acc::execute()
{
  //Local variables
  PINOUT tmp_pinout;
  uint32_t addr;
  uint32_t burst;
  uint32_t data;
  uint32_t bw;
  bool wr;
  uint32_t size;

  int essai=0;

  //Initializations
  sl_rdy.write(false);

  //Main thread
  while(1)
  {
    //Wait for request
    if (!sl_req.read()) wait(sl_req.posedge_event());

    //Get request
    tmp_pinout = slave_port.read();
    addr       = tmp_pinout.address; //Address
    burst      = tmp_pinout.burst;   //Size of burst
    bw         = tmp_pinout.bw;      //Size of data
    wr         = tmp_pinout.rw;      //Read/write cmd
    size       = get_word_size ( bw );

    cout<<"ACCELERATOR Execute function call"<<endl;

    //It is a READ request
    if (!wr)
    {
      if (addr==ACCELERATOR_READY_ADDR)
      {
        //Debug
        cout << "ACCELERATOR Wait for the end of the processing at "<<sc_time_stamp()<<endl;

        //Wait the end of the processing
        if (status == CL_ACC_INACTIVE ) tmp_pinout.data = 1;
        else tmp_pinout.data = 0;

        //End of processing
        //tmp_pinout.data = 1;

        //Write answer
        slave_port.write ( tmp_pinout );

        //Handshaking
        sl_rdy.write ( true );
        wait();
        sl_rdy.write ( false );
        wait();
      }
      else
      {
        //Change status
        status = CL_ACC_READ;

        //Debug
        cout << "ACCELERATOR Read at the address "<<hex<<addr<< " at "<<sc_time_stamp()<<endl;

        //Return the requested data
        for (int i = 0; i < burst; i ++)
        {
          wait();
          sl_rdy.write ( false );
          data = this->Read ( addr, bw );

          //Wait 1 cycle between burst beat
          wait();

          //Increment the address for the next beat
          addr += size;

          tmp_pinout.data = data;
          slave_port.write ( tmp_pinout );
          sl_rdy.write ( true );
        }

        wait();
        sl_rdy.write ( false );
        wait();

        //Change status
        status = CL_ACC_INACTIVE;
      }
    }
    //It is a write
    else
    {
      //Get the data to write
      data = tmp_pinout.data;

      //Control part
      if (addr==ACCELERATOR_START_ADDR)
      {
        if (data==1)
        {
          status = CL_ACC_START;

          //Send active signal
          start_processing.notify();

          //Debug
          cout<<"ACCELERATOR: Start processing"<<endl;
        }
        else
        {
          status = CL_ACC_INACTIVE;

          //Debug
          cout<<"ACCELERATOR: Stop processing"<<endl;
        }

        //Handshaking
        sl_rdy.write ( true );
        wait();
        sl_rdy.write ( false );
        wait();
      }
      else
      {
        //Change status
        status = CL_ACC_WRITE;

        //Debug
        cout << "ACCELERATOR Write at the address "
             << hex << addr
             << " the value "
             << data
             << " at "
             << sc_time_stamp()
             << endl;

        //Write the data in the request
        for (int i = 0; i < burst; i ++)
        {
          wait();
          sl_rdy.write ( false );
          this->Write ( addr, data, bw );

          // Wait 1 cycle between burst beat
          wait();

          // Increment the address for the next beat
          addr += size;
          sl_rdy.write ( true );
        }

        wait();
        sl_rdy.write ( false );
        wait();

        //Change status
        status = CL_ACC_INACTIVE;
      }
    }
  }
}

//Execute
void cl_acc::acc_processing()
{
  //Debug
  cout << "ACCELERATOR: START!"<<endl;

  while (1)
  {
    // Wait for an input.
    wait(start_processing);

    std::cout << "Processing request." << std::endl;
    uint32_t const& input = Read(INPUT_ADDR, MEM_WORD);
    // Send value to FIR.
    std::cout << "FIR input: " << reinterpret_cast<int const&>(input) << std::endl;
    fir_input.write(reinterpret_cast<int const&>(input));

    // Wait for high clock.
    // Because the pipeline need one tick to compute the pipelined result.
    // This will put a latency between input and output.
    wait();

    // Get FIR(t-6) result.
    int const& output = fir_output.read();
    Write(OUTPUT_ADDR, reinterpret_cast<uint32_t const&>(output),MEM_WORD);

    // Return in inactive mode.
    status = CL_ACC_INACTIVE;

    std::cout << "Finish to write input in fir." << std::endl;
  }

  //Debug
  cout << "ACCELERATOR: DONE!"<<endl;
}

