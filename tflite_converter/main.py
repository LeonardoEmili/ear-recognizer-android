import tensorflow as tf


def load_pb(path_to_pb):
    with tf.io.gfile.GFile(path_to_pb, "rb") as f:
        graph_def = tf.compat.v1.GraphDef()
        graph_def.ParseFromString(f.read())
    with tf.Graph().as_default() as graph:
        tf.import_graph_def(graph_def, name='')
        return graph

model = load_pb('model-stage1/saved_model.pb')


'''
# Convert the model
converter = tf.lite.TFLiteConverter.from_saved_model("model-stage1") # path to the SavedModel directory
tflite_model = converter.convert()

# Save the model.
with open('model-stage1/model-stage1.tflite', 'wb') as f:
  f.write(tflite_model)'''


'''
export_dir = "model-stage1"
serving_input_fn = tf.estimator.export.build_parsing_serving_input_receiver_fn(
  tf.feature_column.make_parse_example_spec(feat_cols))

estimator.export_saved_model(export_dir, serving_input_fn)
'''

'''
# Convert the model.
saved_model_obj = tf.saved_model.load("model-stage1")
print(saved_model_obj.signatures.keys())

concrete_func = saved_model_obj.signatures['serving_default']

converter = tf.lite.TFLiteConverter.from_concrete_functions([concrete_func])

print(saved_model_obj.signatures.keys())
converter.optimizations = [tf.lite.Optimize.DEFAULT]
converter.experimental_new_converter = True
#converter.target_spec.supported_ops = [tf.lite.OpsSet.TFLITE_BUILTINS,
 #                                      tf.lite.OpsSet.SELECT_TF_OPS]

tflite_model = converter.convert()

# Save the model.
with open('model-stage1/model-stage1.tflite', 'wb') as f:
  f.write(tflite_model)'''


