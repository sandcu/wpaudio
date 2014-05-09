
using Microsoft.Xna.Framework.Audio;
using System;
using System.Collections.Generic;
using System.IO;
using System.IO.IsolatedStorage;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text;
using System.Threading;
using System.Windows;
using System.Windows.Threading;
using Windows.Storage.Streams;

namespace LameDemo
{
    public class PCMStreamPlayer
    {
        IsolatedStorageFileStream PlayStream;
        public PCMStreamPlayer()
        {

            audioBufferList = new byte[AudioBufferCount][];
            for (int i = 0; i < audioBufferList.Length; ++i)
            {
                audioBufferList[i] = new byte[ChannelCount * BytesPerSample * AudioBufferSize];
            }
            if (dynamicSound != null)
            {
                dynamicSound.Stop();
                dynamicSound.Dispose();
            }
            dynamicSound = new DynamicSoundEffectInstance(44100, AudioChannels.Stereo);
            dynamicSound.BufferNeeded += BufferNeeded;
         
        }
        int AudioBufferCount = 4;
        int AudioBufferSize = 4096;
        int ChannelCount = 2;
        int BytesPerSample = 2;
        byte[][] audioBufferList;
        DynamicSoundEffectInstance dynamicSound;
        BinaryReader br;
        private int CurrentFillBufferIndex = 0;
        private int CurrentPlayBufferIndex;
     
        int readPostion = 0;

        public void Play()
        {
            using (IsolatedStorageFile isf = IsolatedStorageFile.GetUserStoreForApplication())
            {
                PlayStream = isf.OpenFile("/decoded_pcm.pcm", FileMode.Open);
            }
            br = new BinaryReader(PlayStream);
            MessageBox.Show(PlayStream.Length+"");
            CurrentFillBufferIndex = 0;
            CurrentPlayBufferIndex = 0;
            readPostion = 0;
            FillBuffer();
            SubmitBuffer();
            dynamicSound.Play();
        }
        public void Stop()
        {

            if (dynamicSound != null)
            {
                dynamicSound.Stop();
            }

        }
        void BufferNeeded(object sender, EventArgs args)
        {
            FillBuffer();
            SubmitBuffer();
        }

        void SubmitBuffer()
        {

            DynamicSoundEffectInstance currentSound = dynamicSound;
            if (currentSound != null)
            {
                currentSound.SubmitBuffer(audioBufferList[CurrentPlayBufferIndex]);
                if ((++CurrentPlayBufferIndex >= audioBufferList.Length))
                {
                    CurrentPlayBufferIndex = 0;
                }
            }

        }
        public EventHandler beginPlay;
        void FillBuffer()
        {

            byte[] destinationBuffer = audioBufferList[CurrentFillBufferIndex];
            if (++CurrentFillBufferIndex >= audioBufferList.Length)
            {
                CurrentFillBufferIndex = 0;
            }
            if (br.BaseStream.Length > destinationBuffer.Length)
            {

                if (readPostion >= br.BaseStream.Length)
                {
                    Stop();
                    return;
                }
                long postion = br.BaseStream.Position;
                br.BaseStream.Position = readPostion;
                readPostion += br.Read(destinationBuffer, 0, destinationBuffer.Length);
                br.BaseStream.Position = postion;
            }
        }

    }

}
