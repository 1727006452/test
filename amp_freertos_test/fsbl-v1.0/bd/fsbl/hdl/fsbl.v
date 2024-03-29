module fsbl (
    DDR_ras_n,
    DDR_cas_n,
    DDR_cke,
    DDR_ck_n,
    DDR_ck_p,
    DDR_cs_n,
    DDR_reset_n,
    DDR_odt,
    DDR_we_n,
    DDR_ba,
    DDR_addr,
    DDR_dm,
    DDR_dq,
    DDR_dqs_n,
    DDR_dqs_p,
    FIXED_IO_mio,
    FIXED_IO_ps_porb,
    FIXED_IO_ddr_vrn,
    FIXED_IO_ps_srstb,
    FIXED_IO_ddr_vrp,
    FIXED_IO_ps_clk);

  inout DDR_ras_n;
  inout DDR_cas_n;
  inout DDR_cke;
  inout DDR_ck_n;
  inout DDR_ck_p;
  inout DDR_cs_n;
  inout DDR_reset_n;
  inout DDR_odt;
  output DDR_we_n;
  inout [2:0]DDR_ba;
  inout [14:0]DDR_addr;
  inout [3:0]DDR_dm;
  inout [31:0]DDR_dq;
  inout [3:0]DDR_dqs_n;
  inout [3:0]DDR_dqs_p;
  inout [53:0]FIXED_IO_mio;
  inout FIXED_IO_ps_porb;
  inout FIXED_IO_ddr_vrn;
  inout FIXED_IO_ps_srstb;
  inout FIXED_IO_ddr_vrp;
  inout FIXED_IO_ps_clk;

  wire [14:0]processing_system7_0_DDR_Addr;
  wire [2:0]processing_system7_0_DDR_BankAddr;
  wire processing_system7_0_DDR_CAS_n;
  wire processing_system7_0_DDR_CKE;
  wire processing_system7_0_DDR_CS_n;
  wire processing_system7_0_DDR_Clk;
  wire processing_system7_0_DDR_Clk_n;
  wire [3:0]processing_system7_0_DDR_DM;
  wire [31:0]processing_system7_0_DDR_DQ;
  wire [3:0]processing_system7_0_DDR_DQS;
  wire [3:0]processing_system7_0_DDR_DQS_n;
  wire processing_system7_0_DDR_DRSTB;
  wire processing_system7_0_DDR_ODT;
  wire processing_system7_0_DDR_RAS_n;
  wire processing_system7_0_DDR_VRN;
  wire processing_system7_0_DDR_VRP;
  wire processing_system7_0_DDR_WEB;
  wire [53:0]processing_system7_0_MIO;
  wire processing_system7_0_PS_CLK;
  wire processing_system7_0_PS_PORB;
  wire processing_system7_0_PS_SRSTB;

  assign DDR_we_n = processing_system7_0_DDR_WEB;
  fsbl_processing_system7_0 processing_system7_0 (
        .CAN0_PHY_TX(),
        .CAN0_PHY_RX(1'b0),
        .CAN1_PHY_TX(),
        .CAN1_PHY_RX(1'b0),
        .ENET1_GMII_TX_EN(),
        .ENET1_GMII_TX_ER(),
        .ENET1_MDIO_MDC(),
        .ENET1_MDIO_O(),
        .ENET1_MDIO_T(),
        .ENET1_GMII_TXD(),
        .ENET1_GMII_COL(1'b0),
        .ENET1_GMII_CRS(1'b0),
        .ENET1_GMII_RX_CLK(1'b0),
        .ENET1_GMII_RX_DV(1'b0),
        .ENET1_GMII_RX_ER(1'b0),
        .ENET1_GMII_TX_CLK(1'b0),
        .ENET1_MDIO_I(1'b0),
        .ENET1_GMII_RXD(8'b0),
        .I2C1_SDA_I(1'b0),
        .I2C1_SDA_O(),
        .I2C1_SDA_T(),
        .I2C1_SCL_I(1'b0),
        .I2C1_SCL_O(),
        .I2C1_SCL_T(),
        .PJTAG_TCK(1'b0),
        .PJTAG_TMS(1'b0),
        .PJTAG_TD_I(1'b0),
        .PJTAG_TD_T(),
        .PJTAG_TD_O(),
        .SPI0_SCLK_I(1'b0),
        .SPI0_SCLK_O(),
        .SPI0_SCLK_T(),
        .SPI0_MOSI_I(1'b0),
        .SPI0_MOSI_O(),
        .SPI0_MOSI_T(),
        .SPI0_MISO_I(1'b0),
        .SPI0_MISO_O(),
        .SPI0_MISO_T(),
        .SPI0_SS_I(1'b0),
        .SPI0_SS_O(),
        .SPI0_SS1_O(),
        .SPI0_SS2_O(),
        .SPI0_SS_T(),
        .SPI1_SCLK_I(1'b0),
        .SPI1_SCLK_O(),
        .SPI1_SCLK_T(),
        .SPI1_MOSI_I(1'b0),
        .SPI1_MOSI_O(),
        .SPI1_MOSI_T(),
        .SPI1_MISO_I(1'b0),
        .SPI1_MISO_O(),
        .SPI1_MISO_T(),
        .SPI1_SS_I(1'b0),
        .SPI1_SS_O(),
        .SPI1_SS1_O(),
        .SPI1_SS2_O(),
        .SPI1_SS_T(),
        .TTC0_WAVE0_OUT(),
        .TTC0_WAVE1_OUT(),
        .TTC0_WAVE2_OUT(),
        .TTC1_WAVE0_OUT(),
        .TTC1_WAVE1_OUT(),
        .TTC1_WAVE2_OUT(),
        .WDT_RST_OUT(),
        .M_AXI_GP0_ARESETN(),
        .M_AXI_GP0_ARVALID(),
        .M_AXI_GP0_AWVALID(),
        .M_AXI_GP0_BREADY(),
        .M_AXI_GP0_RREADY(),
        .M_AXI_GP0_WLAST(),
        .M_AXI_GP0_WVALID(),
        .M_AXI_GP0_ARID(),
        .M_AXI_GP0_AWID(),
        .M_AXI_GP0_WID(),
        .M_AXI_GP0_ARBURST(),
        .M_AXI_GP0_ARLOCK(),
        .M_AXI_GP0_ARSIZE(),
        .M_AXI_GP0_AWBURST(),
        .M_AXI_GP0_AWLOCK(),
        .M_AXI_GP0_AWSIZE(),
        .M_AXI_GP0_ARPROT(),
        .M_AXI_GP0_AWPROT(),
        .M_AXI_GP0_ARADDR(),
        .M_AXI_GP0_AWADDR(),
        .M_AXI_GP0_WDATA(),
        .M_AXI_GP0_ARCACHE(),
        .M_AXI_GP0_ARLEN(),
        .M_AXI_GP0_ARQOS(),
        .M_AXI_GP0_AWCACHE(),
        .M_AXI_GP0_AWLEN(),
        .M_AXI_GP0_AWQOS(),
        .M_AXI_GP0_WSTRB(),
        .M_AXI_GP0_ACLK(1'b0),
        .M_AXI_GP0_ARREADY(1'b0),
        .M_AXI_GP0_AWREADY(1'b0),
        .M_AXI_GP0_BVALID(1'b0),
        .M_AXI_GP0_RLAST(1'b0),
        .M_AXI_GP0_RVALID(1'b0),
        .M_AXI_GP0_WREADY(1'b0),
        .M_AXI_GP0_BID(12'b0),
        .M_AXI_GP0_RID(12'b0),
        .M_AXI_GP0_BRESP(2'b0),
        .M_AXI_GP0_RRESP(2'b0),
        .M_AXI_GP0_RDATA(32'b0),
        .M_AXI_GP1_ARESETN(),
        .M_AXI_GP1_ARVALID(),
        .M_AXI_GP1_AWVALID(),
        .M_AXI_GP1_BREADY(),
        .M_AXI_GP1_RREADY(),
        .M_AXI_GP1_WLAST(),
        .M_AXI_GP1_WVALID(),
        .M_AXI_GP1_ARID(),
        .M_AXI_GP1_AWID(),
        .M_AXI_GP1_WID(),
        .M_AXI_GP1_ARBURST(),
        .M_AXI_GP1_ARLOCK(),
        .M_AXI_GP1_ARSIZE(),
        .M_AXI_GP1_AWBURST(),
        .M_AXI_GP1_AWLOCK(),
        .M_AXI_GP1_AWSIZE(),
        .M_AXI_GP1_ARPROT(),
        .M_AXI_GP1_AWPROT(),
        .M_AXI_GP1_ARADDR(),
        .M_AXI_GP1_AWADDR(),
        .M_AXI_GP1_WDATA(),
        .M_AXI_GP1_ARCACHE(),
        .M_AXI_GP1_ARLEN(),
        .M_AXI_GP1_ARQOS(),
        .M_AXI_GP1_AWCACHE(),
        .M_AXI_GP1_AWLEN(),
        .M_AXI_GP1_AWQOS(),
        .M_AXI_GP1_WSTRB(),
        .M_AXI_GP1_ACLK(1'b0),
        .M_AXI_GP1_ARREADY(1'b0),
        .M_AXI_GP1_AWREADY(1'b0),
        .M_AXI_GP1_BVALID(1'b0),
        .M_AXI_GP1_RLAST(1'b0),
        .M_AXI_GP1_RVALID(1'b0),
        .M_AXI_GP1_WREADY(1'b0),
        .M_AXI_GP1_BID(12'b0),
        .M_AXI_GP1_RID(12'b0),
        .M_AXI_GP1_BRESP(2'b0),
        .M_AXI_GP1_RRESP(2'b0),
        .M_AXI_GP1_RDATA(32'b0),
        .S_AXI_GP0_ARESETN(),
        .S_AXI_GP0_ARREADY(),
        .S_AXI_GP0_AWREADY(),
        .S_AXI_GP0_BVALID(),
        .S_AXI_GP0_RLAST(),
        .S_AXI_GP0_RVALID(),
        .S_AXI_GP0_WREADY(),
        .S_AXI_GP0_BRESP(),
        .S_AXI_GP0_RRESP(),
        .S_AXI_GP0_RDATA(),
        .S_AXI_GP0_BID(),
        .S_AXI_GP0_RID(),
        .S_AXI_GP0_ACLK(1'b0),
        .S_AXI_GP0_ARVALID(1'b0),
        .S_AXI_GP0_AWVALID(1'b0),
        .S_AXI_GP0_BREADY(1'b0),
        .S_AXI_GP0_RREADY(1'b0),
        .S_AXI_GP0_WLAST(1'b0),
        .S_AXI_GP0_WVALID(1'b0),
        .S_AXI_GP0_ARBURST(2'b0),
        .S_AXI_GP0_ARLOCK(2'b0),
        .S_AXI_GP0_ARSIZE(3'b0),
        .S_AXI_GP0_AWBURST(2'b0),
        .S_AXI_GP0_AWLOCK(2'b0),
        .S_AXI_GP0_AWSIZE(3'b0),
        .S_AXI_GP0_ARPROT(3'b0),
        .S_AXI_GP0_AWPROT(3'b0),
        .S_AXI_GP0_ARADDR(32'b0),
        .S_AXI_GP0_AWADDR(32'b0),
        .S_AXI_GP0_WDATA(32'b0),
        .S_AXI_GP0_ARCACHE(4'b0),
        .S_AXI_GP0_ARLEN(4'b0),
        .S_AXI_GP0_ARQOS(4'b0),
        .S_AXI_GP0_AWCACHE(4'b0),
        .S_AXI_GP0_AWLEN(4'b0),
        .S_AXI_GP0_AWQOS(4'b0),
        .S_AXI_GP0_WSTRB(4'b0),
        .S_AXI_GP0_ARID(6'b0),
        .S_AXI_GP0_AWID(6'b0),
        .S_AXI_GP0_WID(6'b0),
        .S_AXI_GP1_ARESETN(),
        .S_AXI_GP1_ARREADY(),
        .S_AXI_GP1_AWREADY(),
        .S_AXI_GP1_BVALID(),
        .S_AXI_GP1_RLAST(),
        .S_AXI_GP1_RVALID(),
        .S_AXI_GP1_WREADY(),
        .S_AXI_GP1_BRESP(),
        .S_AXI_GP1_RRESP(),
        .S_AXI_GP1_RDATA(),
        .S_AXI_GP1_BID(),
        .S_AXI_GP1_RID(),
        .S_AXI_GP1_ACLK(1'b0),
        .S_AXI_GP1_ARVALID(1'b0),
        .S_AXI_GP1_AWVALID(1'b0),
        .S_AXI_GP1_BREADY(1'b0),
        .S_AXI_GP1_RREADY(1'b0),
        .S_AXI_GP1_WLAST(1'b0),
        .S_AXI_GP1_WVALID(1'b0),
        .S_AXI_GP1_ARBURST(2'b0),
        .S_AXI_GP1_ARLOCK(2'b0),
        .S_AXI_GP1_ARSIZE(3'b0),
        .S_AXI_GP1_AWBURST(2'b0),
        .S_AXI_GP1_AWLOCK(2'b0),
        .S_AXI_GP1_AWSIZE(3'b0),
        .S_AXI_GP1_ARPROT(3'b0),
        .S_AXI_GP1_AWPROT(3'b0),
        .S_AXI_GP1_ARADDR(32'b0),
        .S_AXI_GP1_AWADDR(32'b0),
        .S_AXI_GP1_WDATA(32'b0),
        .S_AXI_GP1_ARCACHE(4'b0),
        .S_AXI_GP1_ARLEN(4'b0),
        .S_AXI_GP1_ARQOS(4'b0),
        .S_AXI_GP1_AWCACHE(4'b0),
        .S_AXI_GP1_AWLEN(4'b0),
        .S_AXI_GP1_AWQOS(4'b0),
        .S_AXI_GP1_WSTRB(4'b0),
        .S_AXI_GP1_ARID(6'b0),
        .S_AXI_GP1_AWID(6'b0),
        .S_AXI_GP1_WID(6'b0),
        .S_AXI_HP0_ARREADY(),
        .S_AXI_HP0_AWREADY(),
        .S_AXI_HP0_BVALID(),
        .S_AXI_HP0_RLAST(),
        .S_AXI_HP0_RVALID(),
        .S_AXI_HP0_WREADY(),
        .S_AXI_HP0_BRESP(),
        .S_AXI_HP0_RRESP(),
        .S_AXI_HP0_BID(),
        .S_AXI_HP0_RID(),
        .S_AXI_HP0_RDATA(),
        .S_AXI_HP0_RCOUNT(),
        .S_AXI_HP0_WCOUNT(),
        .S_AXI_HP0_RACOUNT(),
        .S_AXI_HP0_WACOUNT(),
        .S_AXI_HP0_ACLK(1'b0),
        .S_AXI_HP0_ARVALID(1'b0),
        .S_AXI_HP0_AWVALID(1'b0),
        .S_AXI_HP0_BREADY(1'b0),
        .S_AXI_HP0_RDISSUECAP1_EN(1'b0),
        .S_AXI_HP0_RREADY(1'b0),
        .S_AXI_HP0_WLAST(1'b0),
        .S_AXI_HP0_WRISSUECAP1_EN(1'b0),
        .S_AXI_HP0_WVALID(1'b0),
        .S_AXI_HP0_ARBURST(2'b0),
        .S_AXI_HP0_ARLOCK(2'b0),
        .S_AXI_HP0_ARSIZE(3'b0),
        .S_AXI_HP0_AWBURST(2'b0),
        .S_AXI_HP0_AWLOCK(2'b0),
        .S_AXI_HP0_AWSIZE(3'b0),
        .S_AXI_HP0_ARPROT(3'b0),
        .S_AXI_HP0_AWPROT(3'b0),
        .S_AXI_HP0_ARADDR(32'b0),
        .S_AXI_HP0_AWADDR(32'b0),
        .S_AXI_HP0_ARCACHE(4'b0),
        .S_AXI_HP0_ARLEN(4'b0),
        .S_AXI_HP0_ARQOS(4'b0),
        .S_AXI_HP0_AWCACHE(4'b0),
        .S_AXI_HP0_AWLEN(4'b0),
        .S_AXI_HP0_AWQOS(4'b0),
        .S_AXI_HP0_ARID(6'b0),
        .S_AXI_HP0_AWID(6'b0),
        .S_AXI_HP0_WID(6'b0),
        .S_AXI_HP0_WDATA(64'b0),
        .S_AXI_HP0_WSTRB(8'b0),
        .S_AXI_HP1_ARREADY(),
        .S_AXI_HP1_AWREADY(),
        .S_AXI_HP1_BVALID(),
        .S_AXI_HP1_RLAST(),
        .S_AXI_HP1_RVALID(),
        .S_AXI_HP1_WREADY(),
        .S_AXI_HP1_BRESP(),
        .S_AXI_HP1_RRESP(),
        .S_AXI_HP1_BID(),
        .S_AXI_HP1_RID(),
        .S_AXI_HP1_RDATA(),
        .S_AXI_HP1_RCOUNT(),
        .S_AXI_HP1_WCOUNT(),
        .S_AXI_HP1_RACOUNT(),
        .S_AXI_HP1_WACOUNT(),
        .S_AXI_HP1_ACLK(1'b0),
        .S_AXI_HP1_ARVALID(1'b0),
        .S_AXI_HP1_AWVALID(1'b0),
        .S_AXI_HP1_BREADY(1'b0),
        .S_AXI_HP1_RDISSUECAP1_EN(1'b0),
        .S_AXI_HP1_RREADY(1'b0),
        .S_AXI_HP1_WLAST(1'b0),
        .S_AXI_HP1_WRISSUECAP1_EN(1'b0),
        .S_AXI_HP1_WVALID(1'b0),
        .S_AXI_HP1_ARBURST(2'b0),
        .S_AXI_HP1_ARLOCK(2'b0),
        .S_AXI_HP1_ARSIZE(3'b0),
        .S_AXI_HP1_AWBURST(2'b0),
        .S_AXI_HP1_AWLOCK(2'b0),
        .S_AXI_HP1_AWSIZE(3'b0),
        .S_AXI_HP1_ARPROT(3'b0),
        .S_AXI_HP1_AWPROT(3'b0),
        .S_AXI_HP1_ARADDR(32'b0),
        .S_AXI_HP1_AWADDR(32'b0),
        .S_AXI_HP1_ARCACHE(4'b0),
        .S_AXI_HP1_ARLEN(4'b0),
        .S_AXI_HP1_ARQOS(4'b0),
        .S_AXI_HP1_AWCACHE(4'b0),
        .S_AXI_HP1_AWLEN(4'b0),
        .S_AXI_HP1_AWQOS(4'b0),
        .S_AXI_HP1_ARID(6'b0),
        .S_AXI_HP1_AWID(6'b0),
        .S_AXI_HP1_WID(6'b0),
        .S_AXI_HP1_WDATA(64'b0),
        .S_AXI_HP1_WSTRB(8'b0),
        .S_AXI_HP2_ARREADY(),
        .S_AXI_HP2_AWREADY(),
        .S_AXI_HP2_BVALID(),
        .S_AXI_HP2_RLAST(),
        .S_AXI_HP2_RVALID(),
        .S_AXI_HP2_WREADY(),
        .S_AXI_HP2_BRESP(),
        .S_AXI_HP2_RRESP(),
        .S_AXI_HP2_BID(),
        .S_AXI_HP2_RID(),
        .S_AXI_HP2_RDATA(),
        .S_AXI_HP2_RCOUNT(),
        .S_AXI_HP2_WCOUNT(),
        .S_AXI_HP2_RACOUNT(),
        .S_AXI_HP2_WACOUNT(),
        .S_AXI_HP2_ACLK(1'b0),
        .S_AXI_HP2_ARVALID(1'b0),
        .S_AXI_HP2_AWVALID(1'b0),
        .S_AXI_HP2_BREADY(1'b0),
        .S_AXI_HP2_RDISSUECAP1_EN(1'b0),
        .S_AXI_HP2_RREADY(1'b0),
        .S_AXI_HP2_WLAST(1'b0),
        .S_AXI_HP2_WRISSUECAP1_EN(1'b0),
        .S_AXI_HP2_WVALID(1'b0),
        .S_AXI_HP2_ARBURST(2'b0),
        .S_AXI_HP2_ARLOCK(2'b0),
        .S_AXI_HP2_ARSIZE(3'b0),
        .S_AXI_HP2_AWBURST(2'b0),
        .S_AXI_HP2_AWLOCK(2'b0),
        .S_AXI_HP2_AWSIZE(3'b0),
        .S_AXI_HP2_ARPROT(3'b0),
        .S_AXI_HP2_AWPROT(3'b0),
        .S_AXI_HP2_ARADDR(32'b0),
        .S_AXI_HP2_AWADDR(32'b0),
        .S_AXI_HP2_ARCACHE(4'b0),
        .S_AXI_HP2_ARLEN(4'b0),
        .S_AXI_HP2_ARQOS(4'b0),
        .S_AXI_HP2_AWCACHE(4'b0),
        .S_AXI_HP2_AWLEN(4'b0),
        .S_AXI_HP2_AWQOS(4'b0),
        .S_AXI_HP2_ARID(6'b0),
        .S_AXI_HP2_AWID(6'b0),
        .S_AXI_HP2_WID(6'b0),
        .S_AXI_HP2_WDATA(64'b0),
        .S_AXI_HP2_WSTRB(8'b0),
        .S_AXI_HP3_ARREADY(),
        .S_AXI_HP3_AWREADY(),
        .S_AXI_HP3_BVALID(),
        .S_AXI_HP3_RLAST(),
        .S_AXI_HP3_RVALID(),
        .S_AXI_HP3_WREADY(),
        .S_AXI_HP3_BRESP(),
        .S_AXI_HP3_RRESP(),
        .S_AXI_HP3_BID(),
        .S_AXI_HP3_RID(),
        .S_AXI_HP3_RDATA(),
        .S_AXI_HP3_RCOUNT(),
        .S_AXI_HP3_WCOUNT(),
        .S_AXI_HP3_RACOUNT(),
        .S_AXI_HP3_WACOUNT(),
        .S_AXI_HP3_ACLK(1'b0),
        .S_AXI_HP3_ARVALID(1'b0),
        .S_AXI_HP3_AWVALID(1'b0),
        .S_AXI_HP3_BREADY(1'b0),
        .S_AXI_HP3_RDISSUECAP1_EN(1'b0),
        .S_AXI_HP3_RREADY(1'b0),
        .S_AXI_HP3_WLAST(1'b0),
        .S_AXI_HP3_WRISSUECAP1_EN(1'b0),
        .S_AXI_HP3_WVALID(1'b0),
        .S_AXI_HP3_ARBURST(2'b0),
        .S_AXI_HP3_ARLOCK(2'b0),
        .S_AXI_HP3_ARSIZE(3'b0),
        .S_AXI_HP3_AWBURST(2'b0),
        .S_AXI_HP3_AWLOCK(2'b0),
        .S_AXI_HP3_AWSIZE(3'b0),
        .S_AXI_HP3_ARPROT(3'b0),
        .S_AXI_HP3_AWPROT(3'b0),
        .S_AXI_HP3_ARADDR(32'b0),
        .S_AXI_HP3_AWADDR(32'b0),
        .S_AXI_HP3_ARCACHE(4'b0),
        .S_AXI_HP3_ARLEN(4'b0),
        .S_AXI_HP3_ARQOS(4'b0),
        .S_AXI_HP3_AWCACHE(4'b0),
        .S_AXI_HP3_AWLEN(4'b0),
        .S_AXI_HP3_AWQOS(4'b0),
        .S_AXI_HP3_ARID(6'b0),
        .S_AXI_HP3_AWID(6'b0),
        .S_AXI_HP3_WID(6'b0),
        .S_AXI_HP3_WDATA(64'b0),
        .S_AXI_HP3_WSTRB(8'b0),
        .FCLK_CLK3(),
        .FCLK_CLK2(),
        .FCLK_CLK1(),
        .FCLK_CLK0(),
        .FCLK_RESET0_N(),
        .IRQ_F2P(16'b0),
        .Core0_nFIQ(1'b0),
        .Core0_nIRQ(1'b0),
        .Core1_nFIQ(1'b0),
        .Core1_nIRQ(1'b0),
        .MIO(FIXED_IO_mio),
        .DDR_Clk(DDR_ck_p),
        .DDR_Clk_n(DDR_ck_n),
        .DDR_CKE(DDR_cke),
        .DDR_CS_n(DDR_cs_n),
        .DDR_RAS_n(DDR_ras_n),
        .DDR_CAS_n(DDR_cas_n),
        .DDR_WEB(processing_system7_0_DDR_WEB),
        .DDR_BankAddr(DDR_ba),
        .DDR_Addr(DDR_addr),
        .DDR_ODT(DDR_odt),
        .DDR_DRSTB(DDR_reset_n),
        .DDR_DQ(DDR_dq),
        .DDR_DM(DDR_dm),
        .DDR_DQS(DDR_dqs_p),
        .DDR_DQS_n(DDR_dqs_n),
        .DDR_VRN(FIXED_IO_ddr_vrn),
        .DDR_VRP(FIXED_IO_ddr_vrp),
        .PS_SRSTB(FIXED_IO_ps_srstb),
        .PS_CLK(FIXED_IO_ps_clk),
        .PS_PORB(FIXED_IO_ps_porb),
        .IRQ_P2F_DMAC0(),
        .IRQ_P2F_QSPI(),
        .IRQ_P2F_ENET0(),
        .IRQ_P2F_SDIO0(),
        .IRQ_P2F_I2C0(),
        .IRQ_P2F_SPI0(),
        .IRQ_P2F_UART0(),
        .IRQ_P2F_CAN0(),
        .IRQ_P2F_ENET1(),
        .IRQ_P2F_SDIO1(),
        .IRQ_P2F_I2C1(),
        .IRQ_P2F_SPI1(),
        .IRQ_P2F_UART1(),
        .IRQ_P2F_CAN1());
endmodule

module fsbl_processing_system7_0 (
  CAN0_PHY_TX,
  CAN0_PHY_RX,
  CAN1_PHY_TX,
  CAN1_PHY_RX,
  ENET1_GMII_TX_EN,
  ENET1_GMII_TX_ER,
  ENET1_MDIO_MDC,
  ENET1_MDIO_O,
  ENET1_MDIO_T,
  ENET1_GMII_TXD,
  ENET1_GMII_COL,
  ENET1_GMII_CRS,
  ENET1_GMII_RX_CLK,
  ENET1_GMII_RX_DV,
  ENET1_GMII_RX_ER,
  ENET1_GMII_TX_CLK,
  ENET1_MDIO_I,
  ENET1_GMII_RXD,
  I2C1_SDA_I,
  I2C1_SDA_O,
  I2C1_SDA_T,
  I2C1_SCL_I,
  I2C1_SCL_O,
  I2C1_SCL_T,
  PJTAG_TCK,
  PJTAG_TMS,
  PJTAG_TD_I,
  PJTAG_TD_T,
  PJTAG_TD_O,
  SPI0_SCLK_I,
  SPI0_SCLK_O,
  SPI0_SCLK_T,
  SPI0_MOSI_I,
  SPI0_MOSI_O,
  SPI0_MOSI_T,
  SPI0_MISO_I,
  SPI0_MISO_O,
  SPI0_MISO_T,
  SPI0_SS_I,
  SPI0_SS_O,
  SPI0_SS1_O,
  SPI0_SS2_O,
  SPI0_SS_T,
  SPI1_SCLK_I,
  SPI1_SCLK_O,
  SPI1_SCLK_T,
  SPI1_MOSI_I,
  SPI1_MOSI_O,
  SPI1_MOSI_T,
  SPI1_MISO_I,
  SPI1_MISO_O,
  SPI1_MISO_T,
  SPI1_SS_I,
  SPI1_SS_O,
  SPI1_SS1_O,
  SPI1_SS2_O,
  SPI1_SS_T,
  TTC0_WAVE0_OUT,
  TTC0_WAVE1_OUT,
  TTC0_WAVE2_OUT,
  TTC1_WAVE0_OUT,
  TTC1_WAVE1_OUT,
  TTC1_WAVE2_OUT,
  WDT_RST_OUT,
  M_AXI_GP0_ARESETN,
  M_AXI_GP0_ARVALID,
  M_AXI_GP0_AWVALID,
  M_AXI_GP0_BREADY,
  M_AXI_GP0_RREADY,
  M_AXI_GP0_WLAST,
  M_AXI_GP0_WVALID,
  M_AXI_GP0_ARID,
  M_AXI_GP0_AWID,
  M_AXI_GP0_WID,
  M_AXI_GP0_ARBURST,
  M_AXI_GP0_ARLOCK,
  M_AXI_GP0_ARSIZE,
  M_AXI_GP0_AWBURST,
  M_AXI_GP0_AWLOCK,
  M_AXI_GP0_AWSIZE,
  M_AXI_GP0_ARPROT,
  M_AXI_GP0_AWPROT,
  M_AXI_GP0_ARADDR,
  M_AXI_GP0_AWADDR,
  M_AXI_GP0_WDATA,
  M_AXI_GP0_ARCACHE,
  M_AXI_GP0_ARLEN,
  M_AXI_GP0_ARQOS,
  M_AXI_GP0_AWCACHE,
  M_AXI_GP0_AWLEN,
  M_AXI_GP0_AWQOS,
  M_AXI_GP0_WSTRB,
  M_AXI_GP0_ACLK,
  M_AXI_GP0_ARREADY,
  M_AXI_GP0_AWREADY,
  M_AXI_GP0_BVALID,
  M_AXI_GP0_RLAST,
  M_AXI_GP0_RVALID,
  M_AXI_GP0_WREADY,
  M_AXI_GP0_BID,
  M_AXI_GP0_RID,
  M_AXI_GP0_BRESP,
  M_AXI_GP0_RRESP,
  M_AXI_GP0_RDATA,
  M_AXI_GP1_ARESETN,
  M_AXI_GP1_ARVALID,
  M_AXI_GP1_AWVALID,
  M_AXI_GP1_BREADY,
  M_AXI_GP1_RREADY,
  M_AXI_GP1_WLAST,
  M_AXI_GP1_WVALID,
  M_AXI_GP1_ARID,
  M_AXI_GP1_AWID,
  M_AXI_GP1_WID,
  M_AXI_GP1_ARBURST,
  M_AXI_GP1_ARLOCK,
  M_AXI_GP1_ARSIZE,
  M_AXI_GP1_AWBURST,
  M_AXI_GP1_AWLOCK,
  M_AXI_GP1_AWSIZE,
  M_AXI_GP1_ARPROT,
  M_AXI_GP1_AWPROT,
  M_AXI_GP1_ARADDR,
  M_AXI_GP1_AWADDR,
  M_AXI_GP1_WDATA,
  M_AXI_GP1_ARCACHE,
  M_AXI_GP1_ARLEN,
  M_AXI_GP1_ARQOS,
  M_AXI_GP1_AWCACHE,
  M_AXI_GP1_AWLEN,
  M_AXI_GP1_AWQOS,
  M_AXI_GP1_WSTRB,
  M_AXI_GP1_ACLK,
  M_AXI_GP1_ARREADY,
  M_AXI_GP1_AWREADY,
  M_AXI_GP1_BVALID,
  M_AXI_GP1_RLAST,
  M_AXI_GP1_RVALID,
  M_AXI_GP1_WREADY,
  M_AXI_GP1_BID,
  M_AXI_GP1_RID,
  M_AXI_GP1_BRESP,
  M_AXI_GP1_RRESP,
  M_AXI_GP1_RDATA,
  S_AXI_GP0_ARESETN,
  S_AXI_GP0_ARREADY,
  S_AXI_GP0_AWREADY,
  S_AXI_GP0_BVALID,
  S_AXI_GP0_RLAST,
  S_AXI_GP0_RVALID,
  S_AXI_GP0_WREADY,
  S_AXI_GP0_BRESP,
  S_AXI_GP0_RRESP,
  S_AXI_GP0_RDATA,
  S_AXI_GP0_BID,
  S_AXI_GP0_RID,
  S_AXI_GP0_ACLK,
  S_AXI_GP0_ARVALID,
  S_AXI_GP0_AWVALID,
  S_AXI_GP0_BREADY,
  S_AXI_GP0_RREADY,
  S_AXI_GP0_WLAST,
  S_AXI_GP0_WVALID,
  S_AXI_GP0_ARBURST,
  S_AXI_GP0_ARLOCK,
  S_AXI_GP0_ARSIZE,
  S_AXI_GP0_AWBURST,
  S_AXI_GP0_AWLOCK,
  S_AXI_GP0_AWSIZE,
  S_AXI_GP0_ARPROT,
  S_AXI_GP0_AWPROT,
  S_AXI_GP0_ARADDR,
  S_AXI_GP0_AWADDR,
  S_AXI_GP0_WDATA,
  S_AXI_GP0_ARCACHE,
  S_AXI_GP0_ARLEN,
  S_AXI_GP0_ARQOS,
  S_AXI_GP0_AWCACHE,
  S_AXI_GP0_AWLEN,
  S_AXI_GP0_AWQOS,
  S_AXI_GP0_WSTRB,
  S_AXI_GP0_ARID,
  S_AXI_GP0_AWID,
  S_AXI_GP0_WID,
  S_AXI_GP1_ARESETN,
  S_AXI_GP1_ARREADY,
  S_AXI_GP1_AWREADY,
  S_AXI_GP1_BVALID,
  S_AXI_GP1_RLAST,
  S_AXI_GP1_RVALID,
  S_AXI_GP1_WREADY,
  S_AXI_GP1_BRESP,
  S_AXI_GP1_RRESP,
  S_AXI_GP1_RDATA,
  S_AXI_GP1_BID,
  S_AXI_GP1_RID,
  S_AXI_GP1_ACLK,
  S_AXI_GP1_ARVALID,
  S_AXI_GP1_AWVALID,
  S_AXI_GP1_BREADY,
  S_AXI_GP1_RREADY,
  S_AXI_GP1_WLAST,
  S_AXI_GP1_WVALID,
  S_AXI_GP1_ARBURST,
  S_AXI_GP1_ARLOCK,
  S_AXI_GP1_ARSIZE,
  S_AXI_GP1_AWBURST,
  S_AXI_GP1_AWLOCK,
  S_AXI_GP1_AWSIZE,
  S_AXI_GP1_ARPROT,
  S_AXI_GP1_AWPROT,
  S_AXI_GP1_ARADDR,
  S_AXI_GP1_AWADDR,
  S_AXI_GP1_WDATA,
  S_AXI_GP1_ARCACHE,
  S_AXI_GP1_ARLEN,
  S_AXI_GP1_ARQOS,
  S_AXI_GP1_AWCACHE,
  S_AXI_GP1_AWLEN,
  S_AXI_GP1_AWQOS,
  S_AXI_GP1_WSTRB,
  S_AXI_GP1_ARID,
  S_AXI_GP1_AWID,
  S_AXI_GP1_WID,
  S_AXI_HP0_ARREADY,
  S_AXI_HP0_AWREADY,
  S_AXI_HP0_BVALID,
  S_AXI_HP0_RLAST,
  S_AXI_HP0_RVALID,
  S_AXI_HP0_WREADY,
  S_AXI_HP0_BRESP,
  S_AXI_HP0_RRESP,
  S_AXI_HP0_BID,
  S_AXI_HP0_RID,
  S_AXI_HP0_RDATA,
  S_AXI_HP0_RCOUNT,
  S_AXI_HP0_WCOUNT,
  S_AXI_HP0_RACOUNT,
  S_AXI_HP0_WACOUNT,
  S_AXI_HP0_ACLK,
  S_AXI_HP0_ARVALID,
  S_AXI_HP0_AWVALID,
  S_AXI_HP0_BREADY,
  S_AXI_HP0_RDISSUECAP1_EN,
  S_AXI_HP0_RREADY,
  S_AXI_HP0_WLAST,
  S_AXI_HP0_WRISSUECAP1_EN,
  S_AXI_HP0_WVALID,
  S_AXI_HP0_ARBURST,
  S_AXI_HP0_ARLOCK,
  S_AXI_HP0_ARSIZE,
  S_AXI_HP0_AWBURST,
  S_AXI_HP0_AWLOCK,
  S_AXI_HP0_AWSIZE,
  S_AXI_HP0_ARPROT,
  S_AXI_HP0_AWPROT,
  S_AXI_HP0_ARADDR,
  S_AXI_HP0_AWADDR,
  S_AXI_HP0_ARCACHE,
  S_AXI_HP0_ARLEN,
  S_AXI_HP0_ARQOS,
  S_AXI_HP0_AWCACHE,
  S_AXI_HP0_AWLEN,
  S_AXI_HP0_AWQOS,
  S_AXI_HP0_ARID,
  S_AXI_HP0_AWID,
  S_AXI_HP0_WID,
  S_AXI_HP0_WDATA,
  S_AXI_HP0_WSTRB,
  S_AXI_HP1_ARREADY,
  S_AXI_HP1_AWREADY,
  S_AXI_HP1_BVALID,
  S_AXI_HP1_RLAST,
  S_AXI_HP1_RVALID,
  S_AXI_HP1_WREADY,
  S_AXI_HP1_BRESP,
  S_AXI_HP1_RRESP,
  S_AXI_HP1_BID,
  S_AXI_HP1_RID,
  S_AXI_HP1_RDATA,
  S_AXI_HP1_RCOUNT,
  S_AXI_HP1_WCOUNT,
  S_AXI_HP1_RACOUNT,
  S_AXI_HP1_WACOUNT,
  S_AXI_HP1_ACLK,
  S_AXI_HP1_ARVALID,
  S_AXI_HP1_AWVALID,
  S_AXI_HP1_BREADY,
  S_AXI_HP1_RDISSUECAP1_EN,
  S_AXI_HP1_RREADY,
  S_AXI_HP1_WLAST,
  S_AXI_HP1_WRISSUECAP1_EN,
  S_AXI_HP1_WVALID,
  S_AXI_HP1_ARBURST,
  S_AXI_HP1_ARLOCK,
  S_AXI_HP1_ARSIZE,
  S_AXI_HP1_AWBURST,
  S_AXI_HP1_AWLOCK,
  S_AXI_HP1_AWSIZE,
  S_AXI_HP1_ARPROT,
  S_AXI_HP1_AWPROT,
  S_AXI_HP1_ARADDR,
  S_AXI_HP1_AWADDR,
  S_AXI_HP1_ARCACHE,
  S_AXI_HP1_ARLEN,
  S_AXI_HP1_ARQOS,
  S_AXI_HP1_AWCACHE,
  S_AXI_HP1_AWLEN,
  S_AXI_HP1_AWQOS,
  S_AXI_HP1_ARID,
  S_AXI_HP1_AWID,
  S_AXI_HP1_WID,
  S_AXI_HP1_WDATA,
  S_AXI_HP1_WSTRB,
  S_AXI_HP2_ARREADY,
  S_AXI_HP2_AWREADY,
  S_AXI_HP2_BVALID,
  S_AXI_HP2_RLAST,
  S_AXI_HP2_RVALID,
  S_AXI_HP2_WREADY,
  S_AXI_HP2_BRESP,
  S_AXI_HP2_RRESP,
  S_AXI_HP2_BID,
  S_AXI_HP2_RID,
  S_AXI_HP2_RDATA,
  S_AXI_HP2_RCOUNT,
  S_AXI_HP2_WCOUNT,
  S_AXI_HP2_RACOUNT,
  S_AXI_HP2_WACOUNT,
  S_AXI_HP2_ACLK,
  S_AXI_HP2_ARVALID,
  S_AXI_HP2_AWVALID,
  S_AXI_HP2_BREADY,
  S_AXI_HP2_RDISSUECAP1_EN,
  S_AXI_HP2_RREADY,
  S_AXI_HP2_WLAST,
  S_AXI_HP2_WRISSUECAP1_EN,
  S_AXI_HP2_WVALID,
  S_AXI_HP2_ARBURST,
  S_AXI_HP2_ARLOCK,
  S_AXI_HP2_ARSIZE,
  S_AXI_HP2_AWBURST,
  S_AXI_HP2_AWLOCK,
  S_AXI_HP2_AWSIZE,
  S_AXI_HP2_ARPROT,
  S_AXI_HP2_AWPROT,
  S_AXI_HP2_ARADDR,
  S_AXI_HP2_AWADDR,
  S_AXI_HP2_ARCACHE,
  S_AXI_HP2_ARLEN,
  S_AXI_HP2_ARQOS,
  S_AXI_HP2_AWCACHE,
  S_AXI_HP2_AWLEN,
  S_AXI_HP2_AWQOS,
  S_AXI_HP2_ARID,
  S_AXI_HP2_AWID,
  S_AXI_HP2_WID,
  S_AXI_HP2_WDATA,
  S_AXI_HP2_WSTRB,
  S_AXI_HP3_ARREADY,
  S_AXI_HP3_AWREADY,
  S_AXI_HP3_BVALID,
  S_AXI_HP3_RLAST,
  S_AXI_HP3_RVALID,
  S_AXI_HP3_WREADY,
  S_AXI_HP3_BRESP,
  S_AXI_HP3_RRESP,
  S_AXI_HP3_BID,
  S_AXI_HP3_RID,
  S_AXI_HP3_RDATA,
  S_AXI_HP3_RCOUNT,
  S_AXI_HP3_WCOUNT,
  S_AXI_HP3_RACOUNT,
  S_AXI_HP3_WACOUNT,
  S_AXI_HP3_ACLK,
  S_AXI_HP3_ARVALID,
  S_AXI_HP3_AWVALID,
  S_AXI_HP3_BREADY,
  S_AXI_HP3_RDISSUECAP1_EN,
  S_AXI_HP3_RREADY,
  S_AXI_HP3_WLAST,
  S_AXI_HP3_WRISSUECAP1_EN,
  S_AXI_HP3_WVALID,
  S_AXI_HP3_ARBURST,
  S_AXI_HP3_ARLOCK,
  S_AXI_HP3_ARSIZE,
  S_AXI_HP3_AWBURST,
  S_AXI_HP3_AWLOCK,
  S_AXI_HP3_AWSIZE,
  S_AXI_HP3_ARPROT,
  S_AXI_HP3_AWPROT,
  S_AXI_HP3_ARADDR,
  S_AXI_HP3_AWADDR,
  S_AXI_HP3_ARCACHE,
  S_AXI_HP3_ARLEN,
  S_AXI_HP3_ARQOS,
  S_AXI_HP3_AWCACHE,
  S_AXI_HP3_AWLEN,
  S_AXI_HP3_AWQOS,
  S_AXI_HP3_ARID,
  S_AXI_HP3_AWID,
  S_AXI_HP3_WID,
  S_AXI_HP3_WDATA,
  S_AXI_HP3_WSTRB,
  FCLK_CLK3,
  FCLK_CLK2,
  FCLK_CLK1,
  FCLK_CLK0,
  FCLK_RESET0_N,
  IRQ_F2P,
  Core0_nFIQ,
  Core0_nIRQ,
  Core1_nFIQ,
  Core1_nIRQ,
  MIO,
  DDR_Clk,
  DDR_Clk_n,
  DDR_CKE,
  DDR_CS_n,
  DDR_RAS_n,
  DDR_CAS_n,
  DDR_WEB,
  DDR_BankAddr,
  DDR_Addr,
  DDR_ODT,
  DDR_DRSTB,
  DDR_DQ,
  DDR_DM,
  DDR_DQS,
  DDR_DQS_n,
  DDR_VRN,
  DDR_VRP,
  PS_SRSTB,
  PS_CLK,
  PS_PORB,
  IRQ_P2F_DMAC0,
  IRQ_P2F_QSPI,
  IRQ_P2F_ENET0,
  IRQ_P2F_SDIO0,
  IRQ_P2F_I2C0,
  IRQ_P2F_SPI0,
  IRQ_P2F_UART0,
  IRQ_P2F_CAN0,
  IRQ_P2F_ENET1,
  IRQ_P2F_SDIO1,
  IRQ_P2F_I2C1,
  IRQ_P2F_SPI1,
  IRQ_P2F_UART1,
  IRQ_P2F_CAN1
);

  output CAN0_PHY_TX;
  input CAN0_PHY_RX;
  output CAN1_PHY_TX;
  input CAN1_PHY_RX;
  output [0 : 0] ENET1_GMII_TX_EN;
  output [0 : 0] ENET1_GMII_TX_ER;
  output ENET1_MDIO_MDC;
  output ENET1_MDIO_O;
  output ENET1_MDIO_T;
  output [7 : 0] ENET1_GMII_TXD;
  input ENET1_GMII_COL;
  input ENET1_GMII_CRS;
  input ENET1_GMII_RX_CLK;
  input ENET1_GMII_RX_DV;
  input ENET1_GMII_RX_ER;
  input ENET1_GMII_TX_CLK;
  input ENET1_MDIO_I;
  input [7 : 0] ENET1_GMII_RXD;
  input I2C1_SDA_I;
  output I2C1_SDA_O;
  output I2C1_SDA_T;
  input I2C1_SCL_I;
  output I2C1_SCL_O;
  output I2C1_SCL_T;
  input PJTAG_TCK;
  input PJTAG_TMS;
  input PJTAG_TD_I;
  output PJTAG_TD_T;
  output PJTAG_TD_O;
  input SPI0_SCLK_I;
  output SPI0_SCLK_O;
  output SPI0_SCLK_T;
  input SPI0_MOSI_I;
  output SPI0_MOSI_O;
  output SPI0_MOSI_T;
  input SPI0_MISO_I;
  output SPI0_MISO_O;
  output SPI0_MISO_T;
  input SPI0_SS_I;
  output SPI0_SS_O;
  output SPI0_SS1_O;
  output SPI0_SS2_O;
  output SPI0_SS_T;
  input SPI1_SCLK_I;
  output SPI1_SCLK_O;
  output SPI1_SCLK_T;
  input SPI1_MOSI_I;
  output SPI1_MOSI_O;
  output SPI1_MOSI_T;
  input SPI1_MISO_I;
  output SPI1_MISO_O;
  output SPI1_MISO_T;
  input SPI1_SS_I;
  output SPI1_SS_O;
  output SPI1_SS1_O;
  output SPI1_SS2_O;
  output SPI1_SS_T;
  output TTC0_WAVE0_OUT;
  output TTC0_WAVE1_OUT;
  output TTC0_WAVE2_OUT;
  output TTC1_WAVE0_OUT;
  output TTC1_WAVE1_OUT;
  output TTC1_WAVE2_OUT;
  output WDT_RST_OUT;
  output M_AXI_GP0_ARESETN;
  output M_AXI_GP0_ARVALID;
  output M_AXI_GP0_AWVALID;
  output M_AXI_GP0_BREADY;
  output M_AXI_GP0_RREADY;
  output M_AXI_GP0_WLAST;
  output M_AXI_GP0_WVALID;
  output [11 : 0] M_AXI_GP0_ARID;
  output [11 : 0] M_AXI_GP0_AWID;
  output [11 : 0] M_AXI_GP0_WID;
  output [1 : 0] M_AXI_GP0_ARBURST;
  output [1 : 0] M_AXI_GP0_ARLOCK;
  output [2 : 0] M_AXI_GP0_ARSIZE;
  output [1 : 0] M_AXI_GP0_AWBURST;
  output [1 : 0] M_AXI_GP0_AWLOCK;
  output [2 : 0] M_AXI_GP0_AWSIZE;
  output [2 : 0] M_AXI_GP0_ARPROT;
  output [2 : 0] M_AXI_GP0_AWPROT;
  output [31 : 0] M_AXI_GP0_ARADDR;
  output [31 : 0] M_AXI_GP0_AWADDR;
  output [31 : 0] M_AXI_GP0_WDATA;
  output [3 : 0] M_AXI_GP0_ARCACHE;
  output [3 : 0] M_AXI_GP0_ARLEN;
  output [3 : 0] M_AXI_GP0_ARQOS;
  output [3 : 0] M_AXI_GP0_AWCACHE;
  output [3 : 0] M_AXI_GP0_AWLEN;
  output [3 : 0] M_AXI_GP0_AWQOS;
  output [3 : 0] M_AXI_GP0_WSTRB;
  input M_AXI_GP0_ACLK;
  input M_AXI_GP0_ARREADY;
  input M_AXI_GP0_AWREADY;
  input M_AXI_GP0_BVALID;
  input M_AXI_GP0_RLAST;
  input M_AXI_GP0_RVALID;
  input M_AXI_GP0_WREADY;
  input [11 : 0] M_AXI_GP0_BID;
  input [11 : 0] M_AXI_GP0_RID;
  input [1 : 0] M_AXI_GP0_BRESP;
  input [1 : 0] M_AXI_GP0_RRESP;
  input [31 : 0] M_AXI_GP0_RDATA;
  output M_AXI_GP1_ARESETN;
  output M_AXI_GP1_ARVALID;
  output M_AXI_GP1_AWVALID;
  output M_AXI_GP1_BREADY;
  output M_AXI_GP1_RREADY;
  output M_AXI_GP1_WLAST;
  output M_AXI_GP1_WVALID;
  output [11 : 0] M_AXI_GP1_ARID;
  output [11 : 0] M_AXI_GP1_AWID;
  output [11 : 0] M_AXI_GP1_WID;
  output [1 : 0] M_AXI_GP1_ARBURST;
  output [1 : 0] M_AXI_GP1_ARLOCK;
  output [2 : 0] M_AXI_GP1_ARSIZE;
  output [1 : 0] M_AXI_GP1_AWBURST;
  output [1 : 0] M_AXI_GP1_AWLOCK;
  output [2 : 0] M_AXI_GP1_AWSIZE;
  output [2 : 0] M_AXI_GP1_ARPROT;
  output [2 : 0] M_AXI_GP1_AWPROT;
  output [31 : 0] M_AXI_GP1_ARADDR;
  output [31 : 0] M_AXI_GP1_AWADDR;
  output [31 : 0] M_AXI_GP1_WDATA;
  output [3 : 0] M_AXI_GP1_ARCACHE;
  output [3 : 0] M_AXI_GP1_ARLEN;
  output [3 : 0] M_AXI_GP1_ARQOS;
  output [3 : 0] M_AXI_GP1_AWCACHE;
  output [3 : 0] M_AXI_GP1_AWLEN;
  output [3 : 0] M_AXI_GP1_AWQOS;
  output [3 : 0] M_AXI_GP1_WSTRB;
  input M_AXI_GP1_ACLK;
  input M_AXI_GP1_ARREADY;
  input M_AXI_GP1_AWREADY;
  input M_AXI_GP1_BVALID;
  input M_AXI_GP1_RLAST;
  input M_AXI_GP1_RVALID;
  input M_AXI_GP1_WREADY;
  input [11 : 0] M_AXI_GP1_BID;
  input [11 : 0] M_AXI_GP1_RID;
  input [1 : 0] M_AXI_GP1_BRESP;
  input [1 : 0] M_AXI_GP1_RRESP;
  input [31 : 0] M_AXI_GP1_RDATA;
  output S_AXI_GP0_ARESETN;
  output S_AXI_GP0_ARREADY;
  output S_AXI_GP0_AWREADY;
  output S_AXI_GP0_BVALID;
  output S_AXI_GP0_RLAST;
  output S_AXI_GP0_RVALID;
  output S_AXI_GP0_WREADY;
  output [1 : 0] S_AXI_GP0_BRESP;
  output [1 : 0] S_AXI_GP0_RRESP;
  output [31 : 0] S_AXI_GP0_RDATA;
  output [5 : 0] S_AXI_GP0_BID;
  output [5 : 0] S_AXI_GP0_RID;
  input S_AXI_GP0_ACLK;
  input S_AXI_GP0_ARVALID;
  input S_AXI_GP0_AWVALID;
  input S_AXI_GP0_BREADY;
  input S_AXI_GP0_RREADY;
  input S_AXI_GP0_WLAST;
  input S_AXI_GP0_WVALID;
  input [1 : 0] S_AXI_GP0_ARBURST;
  input [1 : 0] S_AXI_GP0_ARLOCK;
  input [2 : 0] S_AXI_GP0_ARSIZE;
  input [1 : 0] S_AXI_GP0_AWBURST;
  input [1 : 0] S_AXI_GP0_AWLOCK;
  input [2 : 0] S_AXI_GP0_AWSIZE;
  input [2 : 0] S_AXI_GP0_ARPROT;
  input [2 : 0] S_AXI_GP0_AWPROT;
  input [31 : 0] S_AXI_GP0_ARADDR;
  input [31 : 0] S_AXI_GP0_AWADDR;
  input [31 : 0] S_AXI_GP0_WDATA;
  input [3 : 0] S_AXI_GP0_ARCACHE;
  input [3 : 0] S_AXI_GP0_ARLEN;
  input [3 : 0] S_AXI_GP0_ARQOS;
  input [3 : 0] S_AXI_GP0_AWCACHE;
  input [3 : 0] S_AXI_GP0_AWLEN;
  input [3 : 0] S_AXI_GP0_AWQOS;
  input [3 : 0] S_AXI_GP0_WSTRB;
  input [5 : 0] S_AXI_GP0_ARID;
  input [5 : 0] S_AXI_GP0_AWID;
  input [5 : 0] S_AXI_GP0_WID;
  output S_AXI_GP1_ARESETN;
  output S_AXI_GP1_ARREADY;
  output S_AXI_GP1_AWREADY;
  output S_AXI_GP1_BVALID;
  output S_AXI_GP1_RLAST;
  output S_AXI_GP1_RVALID;
  output S_AXI_GP1_WREADY;
  output [1 : 0] S_AXI_GP1_BRESP;
  output [1 : 0] S_AXI_GP1_RRESP;
  output [31 : 0] S_AXI_GP1_RDATA;
  output [5 : 0] S_AXI_GP1_BID;
  output [5 : 0] S_AXI_GP1_RID;
  input S_AXI_GP1_ACLK;
  input S_AXI_GP1_ARVALID;
  input S_AXI_GP1_AWVALID;
  input S_AXI_GP1_BREADY;
  input S_AXI_GP1_RREADY;
  input S_AXI_GP1_WLAST;
  input S_AXI_GP1_WVALID;
  input [1 : 0] S_AXI_GP1_ARBURST;
  input [1 : 0] S_AXI_GP1_ARLOCK;
  input [2 : 0] S_AXI_GP1_ARSIZE;
  input [1 : 0] S_AXI_GP1_AWBURST;
  input [1 : 0] S_AXI_GP1_AWLOCK;
  input [2 : 0] S_AXI_GP1_AWSIZE;
  input [2 : 0] S_AXI_GP1_ARPROT;
  input [2 : 0] S_AXI_GP1_AWPROT;
  input [31 : 0] S_AXI_GP1_ARADDR;
  input [31 : 0] S_AXI_GP1_AWADDR;
  input [31 : 0] S_AXI_GP1_WDATA;
  input [3 : 0] S_AXI_GP1_ARCACHE;
  input [3 : 0] S_AXI_GP1_ARLEN;
  input [3 : 0] S_AXI_GP1_ARQOS;
  input [3 : 0] S_AXI_GP1_AWCACHE;
  input [3 : 0] S_AXI_GP1_AWLEN;
  input [3 : 0] S_AXI_GP1_AWQOS;
  input [3 : 0] S_AXI_GP1_WSTRB;
  input [5 : 0] S_AXI_GP1_ARID;
  input [5 : 0] S_AXI_GP1_AWID;
  input [5 : 0] S_AXI_GP1_WID;
  output S_AXI_HP0_ARREADY;
  output S_AXI_HP0_AWREADY;
  output S_AXI_HP0_BVALID;
  output S_AXI_HP0_RLAST;
  output S_AXI_HP0_RVALID;
  output S_AXI_HP0_WREADY;
  output [1 : 0] S_AXI_HP0_BRESP;
  output [1 : 0] S_AXI_HP0_RRESP;
  output [5 : 0] S_AXI_HP0_BID;
  output [5 : 0] S_AXI_HP0_RID;
  output [63 : 0] S_AXI_HP0_RDATA;
  output [7 : 0] S_AXI_HP0_RCOUNT;
  output [7 : 0] S_AXI_HP0_WCOUNT;
  output [2 : 0] S_AXI_HP0_RACOUNT;
  output [5 : 0] S_AXI_HP0_WACOUNT;
  input S_AXI_HP0_ACLK;
  input S_AXI_HP0_ARVALID;
  input S_AXI_HP0_AWVALID;
  input S_AXI_HP0_BREADY;
  input S_AXI_HP0_RDISSUECAP1_EN;
  input S_AXI_HP0_RREADY;
  input S_AXI_HP0_WLAST;
  input S_AXI_HP0_WRISSUECAP1_EN;
  input S_AXI_HP0_WVALID;
  input [1 : 0] S_AXI_HP0_ARBURST;
  input [1 : 0] S_AXI_HP0_ARLOCK;
  input [2 : 0] S_AXI_HP0_ARSIZE;
  input [1 : 0] S_AXI_HP0_AWBURST;
  input [1 : 0] S_AXI_HP0_AWLOCK;
  input [2 : 0] S_AXI_HP0_AWSIZE;
  input [2 : 0] S_AXI_HP0_ARPROT;
  input [2 : 0] S_AXI_HP0_AWPROT;
  input [31 : 0] S_AXI_HP0_ARADDR;
  input [31 : 0] S_AXI_HP0_AWADDR;
  input [3 : 0] S_AXI_HP0_ARCACHE;
  input [3 : 0] S_AXI_HP0_ARLEN;
  input [3 : 0] S_AXI_HP0_ARQOS;
  input [3 : 0] S_AXI_HP0_AWCACHE;
  input [3 : 0] S_AXI_HP0_AWLEN;
  input [3 : 0] S_AXI_HP0_AWQOS;
  input [5 : 0] S_AXI_HP0_ARID;
  input [5 : 0] S_AXI_HP0_AWID;
  input [5 : 0] S_AXI_HP0_WID;
  input [63 : 0] S_AXI_HP0_WDATA;
  input [7 : 0] S_AXI_HP0_WSTRB;
  output S_AXI_HP1_ARREADY;
  output S_AXI_HP1_AWREADY;
  output S_AXI_HP1_BVALID;
  output S_AXI_HP1_RLAST;
  output S_AXI_HP1_RVALID;
  output S_AXI_HP1_WREADY;
  output [1 : 0] S_AXI_HP1_BRESP;
  output [1 : 0] S_AXI_HP1_RRESP;
  output [5 : 0] S_AXI_HP1_BID;
  output [5 : 0] S_AXI_HP1_RID;
  output [63 : 0] S_AXI_HP1_RDATA;
  output [7 : 0] S_AXI_HP1_RCOUNT;
  output [7 : 0] S_AXI_HP1_WCOUNT;
  output [2 : 0] S_AXI_HP1_RACOUNT;
  output [5 : 0] S_AXI_HP1_WACOUNT;
  input S_AXI_HP1_ACLK;
  input S_AXI_HP1_ARVALID;
  input S_AXI_HP1_AWVALID;
  input S_AXI_HP1_BREADY;
  input S_AXI_HP1_RDISSUECAP1_EN;
  input S_AXI_HP1_RREADY;
  input S_AXI_HP1_WLAST;
  input S_AXI_HP1_WRISSUECAP1_EN;
  input S_AXI_HP1_WVALID;
  input [1 : 0] S_AXI_HP1_ARBURST;
  input [1 : 0] S_AXI_HP1_ARLOCK;
  input [2 : 0] S_AXI_HP1_ARSIZE;
  input [1 : 0] S_AXI_HP1_AWBURST;
  input [1 : 0] S_AXI_HP1_AWLOCK;
  input [2 : 0] S_AXI_HP1_AWSIZE;
  input [2 : 0] S_AXI_HP1_ARPROT;
  input [2 : 0] S_AXI_HP1_AWPROT;
  input [31 : 0] S_AXI_HP1_ARADDR;
  input [31 : 0] S_AXI_HP1_AWADDR;
  input [3 : 0] S_AXI_HP1_ARCACHE;
  input [3 : 0] S_AXI_HP1_ARLEN;
  input [3 : 0] S_AXI_HP1_ARQOS;
  input [3 : 0] S_AXI_HP1_AWCACHE;
  input [3 : 0] S_AXI_HP1_AWLEN;
  input [3 : 0] S_AXI_HP1_AWQOS;
  input [5 : 0] S_AXI_HP1_ARID;
  input [5 : 0] S_AXI_HP1_AWID;
  input [5 : 0] S_AXI_HP1_WID;
  input [63 : 0] S_AXI_HP1_WDATA;
  input [7 : 0] S_AXI_HP1_WSTRB;
  output S_AXI_HP2_ARREADY;
  output S_AXI_HP2_AWREADY;
  output S_AXI_HP2_BVALID;
  output S_AXI_HP2_RLAST;
  output S_AXI_HP2_RVALID;
  output S_AXI_HP2_WREADY;
  output [1 : 0] S_AXI_HP2_BRESP;
  output [1 : 0] S_AXI_HP2_RRESP;
  output [5 : 0] S_AXI_HP2_BID;
  output [5 : 0] S_AXI_HP2_RID;
  output [63 : 0] S_AXI_HP2_RDATA;
  output [7 : 0] S_AXI_HP2_RCOUNT;
  output [7 : 0] S_AXI_HP2_WCOUNT;
  output [2 : 0] S_AXI_HP2_RACOUNT;
  output [5 : 0] S_AXI_HP2_WACOUNT;
  input S_AXI_HP2_ACLK;
  input S_AXI_HP2_ARVALID;
  input S_AXI_HP2_AWVALID;
  input S_AXI_HP2_BREADY;
  input S_AXI_HP2_RDISSUECAP1_EN;
  input S_AXI_HP2_RREADY;
  input S_AXI_HP2_WLAST;
  input S_AXI_HP2_WRISSUECAP1_EN;
  input S_AXI_HP2_WVALID;
  input [1 : 0] S_AXI_HP2_ARBURST;
  input [1 : 0] S_AXI_HP2_ARLOCK;
  input [2 : 0] S_AXI_HP2_ARSIZE;
  input [1 : 0] S_AXI_HP2_AWBURST;
  input [1 : 0] S_AXI_HP2_AWLOCK;
  input [2 : 0] S_AXI_HP2_AWSIZE;
  input [2 : 0] S_AXI_HP2_ARPROT;
  input [2 : 0] S_AXI_HP2_AWPROT;
  input [31 : 0] S_AXI_HP2_ARADDR;
  input [31 : 0] S_AXI_HP2_AWADDR;
  input [3 : 0] S_AXI_HP2_ARCACHE;
  input [3 : 0] S_AXI_HP2_ARLEN;
  input [3 : 0] S_AXI_HP2_ARQOS;
  input [3 : 0] S_AXI_HP2_AWCACHE;
  input [3 : 0] S_AXI_HP2_AWLEN;
  input [3 : 0] S_AXI_HP2_AWQOS;
  input [5 : 0] S_AXI_HP2_ARID;
  input [5 : 0] S_AXI_HP2_AWID;
  input [5 : 0] S_AXI_HP2_WID;
  input [63 : 0] S_AXI_HP2_WDATA;
  input [7 : 0] S_AXI_HP2_WSTRB;
  output S_AXI_HP3_ARREADY;
  output S_AXI_HP3_AWREADY;
  output S_AXI_HP3_BVALID;
  output S_AXI_HP3_RLAST;
  output S_AXI_HP3_RVALID;
  output S_AXI_HP3_WREADY;
  output [1 : 0] S_AXI_HP3_BRESP;
  output [1 : 0] S_AXI_HP3_RRESP;
  output [5 : 0] S_AXI_HP3_BID;
  output [5 : 0] S_AXI_HP3_RID;
  output [63 : 0] S_AXI_HP3_RDATA;
  output [7 : 0] S_AXI_HP3_RCOUNT;
  output [7 : 0] S_AXI_HP3_WCOUNT;
  output [2 : 0] S_AXI_HP3_RACOUNT;
  output [5 : 0] S_AXI_HP3_WACOUNT;
  input S_AXI_HP3_ACLK;
  input S_AXI_HP3_ARVALID;
  input S_AXI_HP3_AWVALID;
  input S_AXI_HP3_BREADY;
  input S_AXI_HP3_RDISSUECAP1_EN;
  input S_AXI_HP3_RREADY;
  input S_AXI_HP3_WLAST;
  input S_AXI_HP3_WRISSUECAP1_EN;
  input S_AXI_HP3_WVALID;
  input [1 : 0] S_AXI_HP3_ARBURST;
  input [1 : 0] S_AXI_HP3_ARLOCK;
  input [2 : 0] S_AXI_HP3_ARSIZE;
  input [1 : 0] S_AXI_HP3_AWBURST;
  input [1 : 0] S_AXI_HP3_AWLOCK;
  input [2 : 0] S_AXI_HP3_AWSIZE;
  input [2 : 0] S_AXI_HP3_ARPROT;
  input [2 : 0] S_AXI_HP3_AWPROT;
  input [31 : 0] S_AXI_HP3_ARADDR;
  input [31 : 0] S_AXI_HP3_AWADDR;
  input [3 : 0] S_AXI_HP3_ARCACHE;
  input [3 : 0] S_AXI_HP3_ARLEN;
  input [3 : 0] S_AXI_HP3_ARQOS;
  input [3 : 0] S_AXI_HP3_AWCACHE;
  input [3 : 0] S_AXI_HP3_AWLEN;
  input [3 : 0] S_AXI_HP3_AWQOS;
  input [5 : 0] S_AXI_HP3_ARID;
  input [5 : 0] S_AXI_HP3_AWID;
  input [5 : 0] S_AXI_HP3_WID;
  input [63 : 0] S_AXI_HP3_WDATA;
  input [7 : 0] S_AXI_HP3_WSTRB;
  output FCLK_CLK3;
  output FCLK_CLK2;
  output FCLK_CLK1;
  output FCLK_CLK0;
  output FCLK_RESET0_N;
  input [15 : 0] IRQ_F2P;
  input Core0_nFIQ;
  input Core0_nIRQ;
  input Core1_nFIQ;
  input Core1_nIRQ;
  inout [53 : 0] MIO;
  inout DDR_Clk;
  inout DDR_Clk_n;
  inout DDR_CKE;
  inout DDR_CS_n;
  inout DDR_RAS_n;
  inout DDR_CAS_n;
  output DDR_WEB;
  inout [2 : 0] DDR_BankAddr;
  inout [14 : 0] DDR_Addr;
  inout DDR_ODT;
  inout DDR_DRSTB;
  inout [31 : 0] DDR_DQ;
  inout [3 : 0] DDR_DM;
  inout [3 : 0] DDR_DQS;
  inout [3 : 0] DDR_DQS_n;
  inout DDR_VRN;
  inout DDR_VRP;
  input PS_SRSTB;
  input PS_CLK;
  input PS_PORB;
  output IRQ_P2F_DMAC0;
  output IRQ_P2F_QSPI;
  output IRQ_P2F_ENET0;
  output IRQ_P2F_SDIO0;
  output IRQ_P2F_I2C0;
  output IRQ_P2F_SPI0;
  output IRQ_P2F_UART0;
  output IRQ_P2F_CAN0;
  output IRQ_P2F_ENET1;
  output IRQ_P2F_SDIO1;
  output IRQ_P2F_I2C1;
  output IRQ_P2F_SPI1;
  output IRQ_P2F_UART1;
  output IRQ_P2F_CAN1;
endmodule

