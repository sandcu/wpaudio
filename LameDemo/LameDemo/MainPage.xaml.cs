using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Navigation;
using Microsoft.Phone.Controls;
using Microsoft.Phone.Shell;

using System.Windows.Threading;

using System.IO;
using Windows.Storage.Streams;
using System.Runtime.InteropServices.WindowsRuntime;

using System.IO.IsolatedStorage;
using System.Windows.Media;

using System.Windows.Controls.Primitives;

using System.Threading;

using System.Diagnostics;
using SandCu_Audio;
namespace LameDemo
{
    public partial class MainPage : PhoneApplicationPage
    {
        // 构造函数
        public MainPage()
        {
            InitializeComponent();
            RhythmStream= Application.GetResourceStream(new Uri("badapple.mp3",UriKind.Relative)).Stream;
            Mp3bytes = new byte[RhythmStream.Length];
            RhythmStream.Position = 0;
            RhythmStream.Read(Mp3bytes, 0, Mp3bytes.Length);
            
            RhythmStream.Close();
        }
        MediaElement melement = new MediaElement();
        IsolatedStorageFileStream PCMStream;
        Stream RhythmStream;
        PCMStreamPlayer player = new PCMStreamPlayer();
        byte[] Mp3bytes;
        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            base.OnNavigatedTo(e);
            TitlePanel.Children.Add(melement);
        }
        private void decode(object sender, RoutedEventArgs e)
        {
            IsolatedStorageFile isf = IsolatedStorageFile.GetUserStoreForApplication();
            LibmadWrapper Libmad = new LibmadWrapper();
            
            IBuffer buffer = WindowsRuntimeBufferExtensions.AsBuffer(Mp3bytes, 0, Mp3bytes.Length);
            PCMStream = isf.CreateFile("decoded_pcm.pcm");
           
            bool init = Libmad.DecodeMp32Pcm_Init(buffer);
            if (init)
            {
                List<short> samples = new List<short>();
                RawPCMContent rpcc = null;
                try
                {
                    while ((rpcc = Libmad.ReadSample()).count != 0)
                    {
                        short[] shortBytes = rpcc.PCMData.ToArray<short>();
                        byte[] rawbytes = new byte[shortBytes.Length * 2];
                        for (int i = 0; i < shortBytes.Length; i++)
                        {
                            rawbytes[2 * i] = (byte)shortBytes[i];
                            rawbytes[2 * i + 1] = (byte)(shortBytes[i] >> 8);
                        }
                         PCMStream.Write(rawbytes, 0, rawbytes.Length);
                    }
                    PCMStream.Flush();
                    PCMStream.Close();
                    PCMStream.Dispose();
                    MessageBox.Show("over");
                    Libmad.CloseFile();    
                }
                catch (Exception exception)
                {
                    MessageBox.Show(exception.Message);
                }
            }
            isf.Dispose();
        }

        int GetSampleRate()
        {
            LibmadWrapper Libmad = new LibmadWrapper();
       
                if (RhythmStream != null)
                {
                    long currentPosition = RhythmStream.Position;
                    Mp3bytes = new byte[RhythmStream.Length];
                    RhythmStream.Position = 0;
                    RhythmStream.Read(Mp3bytes, 0, Mp3bytes.Length);
                    RhythmStream.Position = currentPosition;
                }
    
            IBuffer buffer = WindowsRuntimeBufferExtensions.AsBuffer(Mp3bytes, 0, Mp3bytes.Length);
            if (Libmad.DecodeMp32Pcm_Init(buffer))
            {
                int result = Libmad.ReadSamplesForSampleRate();
                Libmad.CloseFile();
                return result;
            }
            else
            {
                return -1;
            }
        }
        private async void encode(object sender, RoutedEventArgs e)
        {
           // player.Stop();
            LameWrapper lame = new LameWrapper();
            lame.InitialLame(44100, 44100, 2, 5);
            IsolatedStorageFile isf = IsolatedStorageFile.GetUserStoreForApplication();
            using (IsolatedStorageFileStream WorkStream = isf.CreateFile("encoded_mp3.mp3"))
            {
                using (PCMStream = isf.OpenFile("/decoded_pcm.pcm", FileMode.Open))
                {
                    int totalread=0;
                    while (PCMStream.Position<PCMStream.Length)
                    {
                        byte[] rawbytes=new byte[65536];

                        int actualcount=PCMStream.Read(rawbytes, 0, 65536);
                      //  MessageBox.Show("read:" + actualcount);
                        totalread += actualcount;
                        PCMStream.Position = totalread;
                        IBuffer PCMSampleBuffer = WindowsRuntimeBufferExtensions.AsBuffer(rawbytes, 0, rawbytes.Length);
                     
                        CompressedMp3Content citem = await lame.EncodePcm2Mp3(PCMSampleBuffer);
                      //  MessageBox.Show(citem.Mp3Data.Length+"");
                        WorkStream.Write(citem.Mp3Data, 0, citem.Mp3Data.Length);
                        WorkStream.Flush();
                        statustb.Text = "position: " + PCMStream.Position + " total: " + PCMStream.Length ;
                    }
                }
            }

            isf.Dispose();
            lame.CloseLame();
        }

        private void playpcm(object sender, RoutedEventArgs e)
        {
           
            player.Play();
        }
        private void playmp3(object sender, RoutedEventArgs e)
        {
            IsolatedStorageFile isf = IsolatedStorageFile.GetUserStoreForApplication();
            Stream mp3= isf.OpenFile("/encoded_mp3.mp3", FileMode.Open);
            melement.SetSource(mp3);
            melement.Play();
            isf.Dispose();
        }
    }
}