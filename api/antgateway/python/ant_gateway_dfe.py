# Copyright (c) 2017-2021 SKKU ESLAB, and contributors. All rights reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# DFE: DNN Fragment Engine

import tensorflow as tf
import numpy as np

from keras.preprocessing import image
from keras.applications import mobilenet


def dfe_load_and_preprocess_image(img_path):
    img = image.load_img(img_path, target_size=(224, 224))
    img_array = image.img_to_array(img)
    img_array = np.expand_dims(img_array, axis=0)
    input_tensor = mobilenet.preprocess_input(img_array)
    return input_tensor


def dfe_load(model_name, num_fragments):
    # load model fragments
    fragment_files = []
    for i in range(num_fragments):
        fragment_files.append("{}-{}.tflite".format(model_name, i))

    # Load the TFLite model and allocate tensors.
    interpreters = []
    for fragment_file in fragment_files:
        interpreter = tf.lite.Interpreter(
            model_path=fragment_file, num_threads=4)
        interpreter.allocate_tensors()
        interpreters.append(interpreter)
    return interpreters


def dfe_execute(interpreters, input_tensor, start_layer_num, end_layer_num):
    # Input tensor conversion (bytes -> ndarray)
    input_shape = interpreters[start_layer_num].get_input_details()[0]['shape']
    input_tensor = np.frombuffer(input_tensor, dtype=np.float32)
    input_tensor = input_tensor.reshape(input_shape)

    prev_output_tensor = input_tensor.astype(np.float32)
    for i in range(start_layer_num, end_layer_num + 1):
        interpreter = interpreters[i]
        # Get input and output tensors.
        input_details = interpreter.get_input_details()
        output_details = interpreter.get_output_details()

        # Get input tensor
        interpreter.set_tensor(input_details[0]['index'], prev_output_tensor)

        # Run
        interpreter.invoke()

        # Get output tensor
        output_data = interpreter.get_tensor(output_details[0]['index'])
        prev_output_tensor = output_data

    # Output tensor conversion (ndarray -> bytes)
    output_tensor = prev_output_tensor.tobytes()
    return output_tensor
