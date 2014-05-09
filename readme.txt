while build the demo project , u'd better build the libmad project first , then build the audio project,after all builds compeleted,build the main project.

demo usage:
1.press the <decode to pcm> button,the app will decode an build-in mp3 file as a two way pcm file into  local storage as decoded_pcm.pcm.
2.after the process compeleted,press the <play pcm> button ,the phone gonna play the decoded pcm file.
3.press the <encoded to mp3> button,the app will encode the decoded pcm file back to an mp3 file called encoded_mp3.mp3,certainly,it will be saved at the root of local storage.
4.press the <play mp3> button ,the phone gonna play the encoded mp3 file.


ps:
it's just a demo ,so i didn't add any thread lock to ensure the app goes well.
the audio project and libmad project is already tested.

more details:
http://www.cnblogs.com/bader/archive/2013/01/25/2876881.html
