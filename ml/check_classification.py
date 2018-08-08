# TensorFlow and tf.keras
import tensorflow as tf
import random
import math
from tensorflow import keras

# Helper libraries
import numpy as np
import matplotlib.pyplot as plt

print(tf.__version__)

NOT_USP = 0
IS_USP = 1

def load_file(filename, test_stride = 2):

    f = open(filename, "r");

    lines = f.readlines()
    curr_line = 0;
    
    s = int(lines[curr_line].strip())
    curr_line += 1
    k = int(lines[curr_line].strip())
    curr_line += 1
    print("s = %d, k = %d\n" % (s, k))

    num_images = (len(lines) - 2) // (s + 1)

    num_test = num_images // 2
    num_train = num_images - num_test
    curr_test = 0
    curr_train = 0
    num = 0
    
    train_images = np.zeros((num_train,s,k))
    train_labels = np.zeros((num_train,))
    test_images = np.zeros((num_test,s,k))
    test_labels = np.zeros((num_test,))
    num_IS_USP = 0
    num_NOT_USP = 0

    
    while curr_line < len(lines):
        #if num % 1234 == 0:
        #    print("\r %13.8f%%" % (100.0 * num / num_images), end='')
        line = lines[curr_line].strip()
        curr_line +=1
        
        if (line == "NOT_USP"):
            label = NOT_USP
            num_NOT_USP += 1
        elif (line == "IS_USP"):
            label = IS_USP
            num_IS_USP += 1
        else:
            raise

        if num % test_stride == 0:
            test_labels[curr_test] = label
        else:
            train_labels[curr_train] = label
        
        image = np.zeros((1,s,k))
        for i in range(s):
            line = lines[curr_line].strip()
            curr_line +=1
            for j in range(k):
                c = line[j]
                if c == "1":
                    val = 0
                elif c == "2":
                    val = 0.5
                elif c == "3":
                    val = 1
                else:
                    raise

                if num % test_stride == 0:
                    test_images[curr_test][i][j]
                else:
                    train_images[curr_train][i][j]

        if num % test_stride == 0:
            curr_test += 1
        else:
            curr_train += 1
        num += 1
            
    return (s, k, num_NOT_USP, num_IS_USP, train_images, train_labels, test_images, test_labels)
    

(s, k, num_NOT_USP, num_IS_USP, train_images, train_labels, test_images, test_labels) = load_file("4-by-6.puz")

class_names = ['NOT_USP', 'IS_USP']

print("train_images.shape = ", train_images.shape)
print("train num", len(train_labels))
print("test num", len(test_labels))
#print(train_images)
#print(train_labels)
#test_images.shape
#len(test_labels)

'''
plt.figure()
plt.imshow(train_images[0])
plt.colorbar()
plt.gca().grid(False)

plt.figure(figsize=(10,10))
for i in range(25):
    plt.subplot(5,5,i+1)
    plt.xticks([])
    plt.yticks([])
    plt.grid('off')
    plt.imshow(train_images[i], cmap=plt.cm.binary)
    plt.xlabel(class_names[train_labels[i]])
'''

with tf.device('/gpu:0'):
    model = keras.Sequential([
        keras.layers.Flatten(input_shape=(s, k)),
        keras.layers.Dense(1280, activation=tf.nn.relu),
        keras.layers.Dense(640, activation=tf.nn.relu),
        keras.layers.Dense(320, activation=tf.nn.relu),
        keras.layers.Dense(2, activation=tf.nn.softmax)
    ])

    model.compile(optimizer=tf.train.AdamOptimizer(), 
                  loss='sparse_categorical_crossentropy',
                  metrics=['accuracy'])


    model.fit(train_images, train_labels, epochs=10000, verbose=2, validation_data=(test_images, test_labels))
    
    test_loss, test_acc = model.evaluate(test_images, test_labels)

print('Test accuracy:', test_acc)

print("NOT_USP = %d, IS_USP = %d\n" % (num_NOT_USP, num_IS_USP))

'''
predictions = model.predict(test_images)
predictions[0]
np.argmax(predictions[0])
test_labels[0]

plt.figure(figsize=(10,10))
for i in range(25):
    plt.subplot(5,5,i+1)
    plt.xticks([])
    plt.yticks([])
    plt.grid('off')
    plt.imshow(test_images[i], cmap=plt.cm.binary)
    predicted_label = np.argmax(predictions[i])
    true_label = test_labels[i]
    if predicted_label == true_label:
      color = 'green'
    else:
      color = 'red'
    plt.xlabel("{} ({})".format(class_names[predicted_label], 
                                  class_names[true_label]),
                                  color=color)

img = test_images[0]

print(img.shape)
img = (np.expand_dims(img,0))

print(img.shape)
predictions = model.predict(img)

print(predictions)
prediction = predictions[0]

np.argmax(prediction)
'''
