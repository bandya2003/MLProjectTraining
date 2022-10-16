def build_model_pipeline():
    from sklearn.pipeline import Pipeline
    
    logger.info("Running build_model_pipeline")
    pipeline = Pipeline(
        steps=[
            ('featurization', generate_data_transformation_config()),
            ('preproc', generate_preprocessor_config()),
            ('model', generate_algorithm_config()),
        ]
    )
    
    return pipeline


def train_model(X, y, sample_weights):
    
    logger.info("Running train_model")
    model_pipeline = build_model_pipeline()
    
    model = model_pipeline.fit(X, y)
    return model



def main(training_dataset_id=None):
    from azureml.core.run import Run
    
    # The following code is for when running this code as part of an AzureML script run.
    run = Run.get_context()
    setup_instrumentation(run)
    
    df = get_training_dataset(training_dataset_id)
    X, y, sample_weights = prepare_data(df)
    split_ratio = 0.1
    try:
        (X_train, y_train, sample_weights_train), (X_valid, y_valid, sample_weights_valid) = split_dataset(X, y, sample_weights, split_ratio, should_stratify=True)
    except Exception:
        (X_train, y_train, sample_weights_train), (X_valid, y_valid, sample_weights_valid) = split_dataset(X, y, sample_weights, split_ratio, should_stratify=False)

    model = train_model(X_train, y_train, sample_weights_train)
    
    metrics = calculate_metrics(model, X, y, sample_weights, X_test=X_valid, y_test=y_valid)
    
    print(metrics)
    for metric in metrics:
        run.log(metric, metrics[metric])
