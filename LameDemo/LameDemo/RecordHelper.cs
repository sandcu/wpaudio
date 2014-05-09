using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Audio;
using System;
using System.IO;
using System.Windows.Threading;
using System.IO.IsolatedStorage;
using System.Diagnostics;
using ChangBa_Audio;
using Windows.Storage.Streams;
using System.Runtime.InteropServices.WindowsRuntime;
namespace sandcu.demo
{

    using AudioStreamEventArgs = ClientAsyncCompletedEventArgs<byte[]>;

    public class MicrophoneWrapper
    {
        private static MicrophoneWrapper _instance;
        public int RecordLevel = 0;
        public static MicrophoneWrapper Instance
        {
            get { return _instance ?? (_instance = new MicrophoneWrapper()); }
        }
        private const int MicrophoneBufferDuration = 1000; // In Millisecond

        private Microphone _microphone = Microphone.Default;
        private MemoryStream _stream = null;
        IsolatedStorageFile isf;
        private string tempFileKey = null;
        IsolatedStorageFileStream tempFileStream;
        private byte[] _buffer;
        private TimeSpan _sampleDuration;

        // private DispatcherTimer _dispatcherTimer = null;

        public event EventHandler<AudioStreamEventArgs> AudioRecordStopped;

        /// <summary>
        /// Create a microphone instance and set it ready to record audio.
        /// </summary>
        private MicrophoneWrapper()
        {
            _microphone.BufferDuration = TimeSpan.FromMilliseconds(MicrophoneBufferDuration);
            _buffer = new byte[_microphone.GetSampleSizeInBytes(_microphone.BufferDuration)];

            _microphone.BufferReady += (s, e) =>
            {
                GetRecordedData();
            };
        }
        private bool useTempFile = false;
        /// <summary>
        /// Start to record audio.
        /// </summary>
        public void StartRecord()
        {
            _stream = new MemoryStream();
            _sampleDuration = TimeSpan.FromSeconds(0);
            FrameworkDispatcher.Update();
            // _dispatcherTimer.Start();
            _microphone.Start();
        }
        public string PrepareStartRecordToTempFile()
        {
            RecordLevel = 0;
            useTempFile = true;
            isf = IsolatedStorageFile.GetUserStoreForApplication();
            tempFileKey = Guid.NewGuid().ToString() + ".tmp";
            tempFileStream = isf.CreateFile(tempFileKey);
            _sampleDuration = TimeSpan.FromSeconds(0);
            FrameworkDispatcher.Update();
            //_dispatcherTimer.Start();
            return tempFileKey;
        }
        public void StartRecordToTempFile()
        {
            _microphone.Start();
        }
        /// <summary>
        /// Stop the record process. The recorded audio data can be get by the event AudioRecordStopped.
        /// </summary>
        /// <param name="isRecordCancelled">True for cancel this record, or false otherwise</param>
        /// <param name="userToken"></param>
        public void StopRecord(bool isRecordCancelled, object userToken)
        {
            GetRecordedData();

            _microphone.Stop();
            // _dispatcherTimer.Stop();

            if (!isRecordCancelled)
            {
                byte[] data = _stream.ToArray();
                _sampleDuration = _microphone.GetSampleDuration(data.Length);

                OnRecordStopped(new AudioStreamEventArgs(data, userToken));
            }
            _stream.Close();
            _stream = null;
        }
        public void StopRecordToTempFile(string tempFileKey)
        {
            //if (tempFileKey == this.tempFileKey)
            //{
            GetRecordedData();
            _microphone.Stop();

            isf.Dispose();
            if (tempFileStream != null)
            {
                tempFileStream.Close();
                tempFileStream.Dispose();
                tempFileStream = null;
            }
            this.tempFileKey = null;

            return;
#if DEBUG
            Debug.WriteLine("file stop fail");
#endif
            // }
        }
        private void OnRecordStopped(AudioStreamEventArgs e)
        {
            var handler = AudioRecordStopped;
            if (handler != null)
            {
                handler(this, e);
            }
        }

        /// <summary>
        /// Transfer recorded data to current stream.
        /// </summary>
        async private void GetRecordedData()
        {
            _microphone.GetData(_buffer);

            if (_buffer.Length >= 2)
            {
                RecordLevel = ByteToLevel(_buffer);
            }
            else
            {
                RecordLevel = 0;
            }
            if (!useTempFile)
            {
                if (_stream != null)
                {
                    _stream.Write(_buffer, 0, _buffer.Length);
                }
            }
            else
            {
                if (tempFileStream != null)
                {

                    //int bufferCount= _buffer.Length%2==0?_buffer.Length*2:(_buffer.Length-1)*2;
                    //byte[] recordDataBuffer = new byte[bufferCount];
                    //for (int i = 0; i<bufferCount/2; i+=2) 
                    //{
                    //    recordDataBuffer[2 * i] = _buffer[i];
                    //    recordDataBuffer[2 * (i + 1)] = _buffer[i];
                    //    recordDataBuffer[2 * i + 1] = _buffer[i + 1];
                    //    recordDataBuffer[2 * (i + 1) + 1] = _buffer[i + 1];
                    //}
                    //IBuffer PCMSampleBuffer = WindowsRuntimeBufferExtensions.AsBuffer(recordDataBuffer, 0, recordDataBuffer.Length);
                    //CompressedMp3Content citem = await lame.EncodePcm2Mp3(PCMSampleBuffer);
                    //tempFileStream.Write(citem.Mp3Data, 0, citem.Mp3Data.Length);
                    tempFileStream.Write(_buffer, 0, _buffer.Length);
                }
            }
            Array.Clear(_buffer, 0, _buffer.Length);
        }
        private int ByteToLevel(byte[] buffer)
        {
            double rms = 0;
            ushort byte1 = 0;
            ushort byte2 = 0;
            short value = 0;
            int volume = 0;
            rms = (short)(byte1 | (byte2 << 8));

            for (int i = 0; i < buffer.Length - 1; i += 2)
            {
                byte1 = buffer[i];
                byte2 = buffer[i + 1];

                value = (short)(byte1 | (byte2 << 8));
                rms += Math.Pow(value, 2);
            }
            rms /= (double)(buffer.Length / 2);
            volume = (int)Math.Floor(Math.Sqrt(rms));
            if (volume != 0)
            {
                return covertVolume(volume);// = volume / (short.MaxValue / 20);
            }
            return volume;

        }
        int covertVolume(int orgVol)
        {
            return (orgVol / (short.MaxValue / 20)) / 2;

        }
        /// <summary>
        /// Get the current microphone's state.
        /// </summary>
        public MicrophoneState State
        {
            get { return _microphone.State; }
        }

        /// <summary>
        /// Get the current microphone's sample rate.
        /// </summary>
        public int SampleRate
        {
            get { return _microphone.SampleRate; }
        }

        /// <summary>
        /// Get time of recorded sound, in second.
        /// </summary>
        public TimeSpan SampleDuration
        {
            get { return _sampleDuration; }
        }
    }
}
