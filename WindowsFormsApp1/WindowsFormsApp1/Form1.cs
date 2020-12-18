using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;

namespace WindowsFormsApp1
{
    public partial class Form1 : Form
    {
        String txt;
        
        public Form1()
        {
            InitializeComponent();
            
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            string[] ports = SerialPort.GetPortNames();
            comboBox1.Items.AddRange(ports);
            comboBox1.SelectedIndex = 0;
            button1.Enabled = false;
            //this.comboBox1.DataSource = ports;
        }

        private void textBox1_TextChanged_1(object sender, EventArgs e)
        {

        }

        private void label1_Click(object sender, EventArgs e)
        {

        }

        private void button1_Click(object sender, EventArgs e)
        {
            button3.Enabled = true;
            button2.Enabled = true;
            button1.Enabled = false;
            comboBox1.Enabled = true;
            textBox1.Enabled = true;
            textBox1.Clear();
            textBox2.Clear();
            txt = "";
            try
            {
                serialPort1.Close();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message, "Message", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void comboBox1_SelectedIndexChanged(object sender, EventArgs e)
        {

        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (String.IsNullOrEmpty(textBox1.Text))
            {
                MessageBox.Show("NIK tidak boleh kosong!", "Error", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            }
            else
            {
                button3.Enabled = false;
                button2.Enabled = false;
                button1.Enabled = true;
                comboBox1.Enabled = false;
                textBox1.Enabled = false;
                try
                {
                    serialPort1.PortName = comboBox1.Text;
                    serialPort1.Open();
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message, "Message", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

        private void button3_Click(object sender, EventArgs e)
        {
            string[] ports = SerialPort.GetPortNames();
            comboBox1.Items.Clear();
            comboBox1.Items.AddRange(ports);
            comboBox1.SelectedIndex = 0;
        }

        private void textBox2_TextChanged(object sender, EventArgs e)
        {
 
        }

        private void serialPort1_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            txt += serialPort1.ReadExisting().ToString();
            SetText(txt.ToString());
            //System.Threading.Thread.Sleep(5000);
            //textBox2.Text = serialPort1.ReadLine();
            //MessageBox.Show(serialPort1.ReadLine(), "Message", MessageBoxButtons.OK, MessageBoxIcon.Warning);
            //serialPort1.Close();
        }

        delegate void SetTextCallback(string text);

        private void SetText(string text)
        {
            // InvokeRequired required compares the thread ID of the
            // calling thread to the thread ID of the creating thread.
            // If these threads are different, it returns true.
            if (this.textBox1.InvokeRequired)
            {
                SetTextCallback d = new SetTextCallback(SetText);
                this.Invoke(d, new object[] { text });
            }
            else
            {
                this.textBox2.Text = text;
            }
        }
    }
}
