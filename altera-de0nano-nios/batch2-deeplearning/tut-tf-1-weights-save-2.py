#!/usr/bin/env python3
# 
# https://www.tensorflow.org/tutorials/quickstart/beginner
# ubuntu bionic: runs on tensorflow 2.0.2. 2.1.0 failed loading libcuda. python 3.6
# windows10: tensorflow 2.2.0. failed cudart64_101.dll is just a warning. python 3.8.2.

import os
import tensorflow as tf
import time

os.environ["CUDA_VISIBLE_DEVICES"] = "-1"

mnist = tf.keras.datasets.mnist
tm01 = time.time()
(x_train, y_train), (x_test, y_test) = mnist.load_data()
tm02 = time.time()
x_train, x_test = x_train / 255.0, x_test/255.0
print(" load_data cost %.3f " % (tm02 - tm01))

print(" model ... ")
model = tf.keras.models.Sequential([
  tf.keras.layers.Flatten(input_shape=(28,28)),
  tf.keras.layers.Dense(128,activation='relu'),
  tf.keras.layers.Dropout(0.2),
  tf.keras.layers.Dense(10)
])

#print(" predictions ... ")
#predictions = model(x_train[:1]).numpy()
#print(" predictions result ")
#print("   %s" % str(predictions))

loss_fn = tf.keras.losses.SparseCategoricalCrossentropy(from_logits=True)
#loss = loss_fn(y_train[:1], predictions).numpy()
#print(" loss %.3f " % loss)

model.compile(optimizer='adam', loss=loss_fn, metrics=['accuracy'])

checkpoint_path = "training_2/cp-{epoch:04d}.ckpt"
checkpoint_dir = os.path.dirname(checkpoint_path)
if False:
    model.fit(x_train, y_train, epochs=2)
    model.save_weights(checkpoint_path.format(epoch=2))
else:
    latest = tf.train.latest_checkpoint(checkpoint_dir)
    model.load_weights(latest)

print(" evaluate ... ")
tm11 = time.time()
evaluated = model.evaluate(x_test, y_test, verbose=2)
tm12 = time.time()
print(" evaluate result ")
print("   %s" % str(evaluated))
print(" evaluate cost %.3f " % (tm12 - tm11))

prob_model = tf.keras.Sequential([
  model, 
  tf.keras.layers.Softmax()
])

def prof_test(the_end=None):
    if the_end is None: the_end = 5
    if the_end < 5: the_end = 5
    if the_end > 10000: the_end = 10000
    return prob_model.predict(x_test[:the_end])
tm21 = time.time()
test_results = prof_test(5)
tm22 = time.time()
test_results = prof_test(500)
tm23 = time.time()
test_results = prof_test(10000)
tm24 = time.time()
print(" probability result ")
print("   len %s" % str( test_results.shape[0] ))
print(" probability cost for 5/500/10k  %.6f %.6f  %.6f  sample time us %.3f " % (
    tm22 - tm21, tm23-tm22, tm24-tm23, (tm24-tm23)*1000*1000/10000))
test_ref_results = [7, 2, 1, 0, 4, 1] # y_test first 6 elements]
def print_ref(test_results, test_ref_results):
    retv = ""
    for i in range(6):
        if i >= len(test_results) or i >= len(test_ref_results): break
        retv += " %.3f" % test_results[i][test_ref_results[i]]
    return retv
print(" probability ref comparison : %s " % print_ref(test_results, test_ref_results))

def prof_train():
    return prob_model.predict(x_train[:60000])
tm33 = time.time()
test_results = prof_train()
tm34 = time.time()
print(" probability result ")
print("   len %s" % str( x_train.shape[0] ))
print(" probability cost %.6f   sample time us %.3f" % (
    tm34 - tm33, (tm34-tm33)*1000*1000/x_train.shape[0]))
test_ref_results = [5, 0, 4, 1, 9, 2] # y_train first 6 elements
print(" probability ref comparison : %s " % print_ref(test_results, test_ref_results))

print("")
print(" model build cost %.3f " % (tm11 - tm02))
print(" total cost       %.3f " % (tm34 - tm01))

print("")
print(" platform computations G per second %.3f" % (28.0*28.0*128*10*x_train.shape[0]/(tm34-tm33)/1000.0/1000.0/1000.0))

#print(" softmax ... ")
#result = tf.nn.softmax(predictions).numpy()
#print(" softmax result ")
#print("   %s" % str(result))


