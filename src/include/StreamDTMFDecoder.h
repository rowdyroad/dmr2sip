#pragma once

#include <exception>
#include <thread>
#include <alsa/asoundlib.h>
#include "DTMFDecoder/DTMFDecoder.h"

namespace Commutator {
	class StreamDTMFDecoder : public DTMFDecoder<>::Handler {
		public:
			class Exception : public std::exception {};
			class DeviceException : public std::exception {};

			class Handler {
				public:
					virtual void OnCode(StreamDTMFDecoder * const sender, uint8_t code)  = 0;
			};
		private:
			volatile bool stop_ = false;
			snd_pcm_t* snd_ = nullptr;
			StreamDTMFDecoder::Handler* handler_ = nullptr;
			std::unique_ptr<std::thread> thread_;
			std::unique_ptr<DTMFDecoder<>> decoder_;

			void run()
			{
				std::vector<int16_t> frames(400);
				while (!stop_) {
					int chunk_length = snd_pcm_readi(snd_, frames.data(), frames.size());
					 if (chunk_length <= 0) {
					 	fprintf (stderr, "read device %d (%s)\n", chunk_length, snd_strerror (chunk_length));
	     				throw new DeviceException();
	    			}
	    			decoder_->decode(frames, chunk_length);
				};
			}
		public:
			void OnCodeBegin(DTMFDecoder<> * const sender, uint8_t code) {
				handler_->OnCode(this, code);
			}
	        void OnCode(DTMFDecoder<> * const sender, uint8_t code, size_t duration) {}
	        void OnCodeEnd(DTMFDecoder<> * const sender, uint8_t code, size_t duration) {}

			StreamDTMFDecoder(Handler* handler, const std::string& device)
				: handler_(handler)
			{
				snd_pcm_hw_params_t *hw_params;
				std::string name = "plug:" + device + "capture";
				int err;
				if ((err = snd_pcm_open (&snd_, name.c_str(), SND_PCM_STREAM_CAPTURE, 0)) < 0) {
					fprintf (stderr, "cannot open audio device %s %d (%s)\n", name.c_str(), err, snd_strerror (err));
					throw new DeviceException();
				}
				snd_pcm_hw_params_malloc (&hw_params);
				snd_pcm_hw_params_any (snd_, hw_params);
				snd_pcm_hw_params_set_access (snd_, hw_params, SND_PCM_ACCESS_RW_INTERLEAVED);
				snd_pcm_hw_params_set_format (snd_, hw_params, SND_PCM_FORMAT_S16_LE);
				snd_pcm_hw_params_set_rate (snd_, hw_params, 8000, 0);
				snd_pcm_hw_params_set_channels (snd_, hw_params, 1);
				snd_pcm_hw_params(snd_, hw_params);
				snd_pcm_hw_params_free (hw_params);
				snd_pcm_prepare (snd_);
				decoder_.reset(new DTMFDecoder<>(this, 8000, 80, 20, -20));
				thread_.reset(new std::thread(&Commutator::StreamDTMFDecoder::run, this));
			}

			~StreamDTMFDecoder()
			{
				stop_ = true;
				thread_->join();
				snd_pcm_close (snd_);
			}
	};
}
