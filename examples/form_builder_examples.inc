<?php
// $Id$

/**
 * @file cck.inc
 * Sample implementation of Form builder hooks for CCK.
 */


/**
 * Temporary implementation of hook_menu, rerouting CCK's UI to Form builder.
 */
function form_builder_form_builder_examples_menu() {
  $items = array();

  $items['form-builder-example'] = array(
    'title' => 'Form builder example',
    'page callback' => 'form_builder_interface',
    'page arguments' => array('form_builder_examples', 'sample'),
    'file' => 'includes/form_builder.admin.inc',
    'file path' => drupal_get_path('module', 'form_builder'),
    'access arguments' => array('access content'),
    'type' => MENU_NORMAL_ITEM,
  );

  $items['form-builder-example/edit'] = array(
    'title' => 'Edit',
    'page callback' => 'form_builder_interface',
    'page arguments' => array('form_builder_examples', 'sample'),
    'file' => 'includes/form_builder.admin.inc',
    'file path' => drupal_get_path('module', 'form_builder'),
    'access arguments' => array('access content'),
    'type' => MENU_DEFAULT_LOCAL_TASK,
  );

  $items['form-builder-example/import'] = array(
    'title' => 'Import',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('form_builder_examples_import'),
    'access callback' => FALSE,
    'access arguments' => array('access content'),
    'type' => MENU_LOCAL_TASK,
  );

  $items['form-builder-example/export'] = array(
    'title' => 'Export',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('form_builder_examples_export'),
    'access arguments' => array('access content'),
    'type' => MENU_LOCAL_TASK,
  );

  return $items;
}

/**
 * Implementation of hook_form_builder_types().
 *
 * Define the supported fields and properties supported by CCK.
 */
function form_builder_examples_form_builder_types() {
  $fields = array();

  $fields['fieldset'] = array(
    'title' => t('Fieldset'),
    'properties' => array(
      'title',
      'description',
      'collapsible',
      'collapsed',
    ),
    'default' => array(
      '#title' => t('New fieldset'),
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
    ),
  );

  $fields['number'] = array(
    'title' => t('Number'),
    'properties' => array(
      'title',
      'description',
      'field_prefix',
      'field_suffix',
      'default_value',
      'required',
      // TODO: how to handle validate functions?
      'number_validate',
    ),
    'validate' => array(
      'number',
    ),
    'default' => array(
      '#title' => t('New number'),
      '#type' => 'textfield',
    ),
  );

  $fields['select'] = array(
    'title' => t('Select list'),
    'properties' => array(
      'title',
      'description',
      'default_value',
      'required',
      'options',
      'multiple',
    ),
    'default' => array(
      '#title' => t('New select list'),
      '#type' => 'select',
      '#options' => array('one' => 'one', 'two' => 'two', 'three' => 'three'),
    ),
  );

  // TODO: Merge checkbox with checkboxes?
  /*
  $fields['checkbox'] = array(
    'title' => t('Checkbox'),
    'properties' => array(
      'title',
      'description',
      'return_value',
      'default_value',
      'required',
     ),
    'default' => array(
      '#title' => t('New checkbox'),
      '#type' => 'checkbox',
    ),
  );
  */

  $fields['checkboxes'] = array(
    'title' => t('Checkboxes'),
    'properties' => array(
      'title',
      'description',
      'default_value',
      'required',
      'options',
      'multiple',
    ),
    'default' => array(
      '#title' => t('New checkboxes'),
      '#type' => 'checkboxes',
      '#options' => array('one' => 'one', 'two' => 'two', 'three' => 'three'),
    ),
  );

  $fields['radios'] = array(
    'title' => t('Radios'),
    'properties' => array(
      'title',
      'description',
      'default_value',
      'required',
      'options',
    ),
    'default' => array(
      '#title' => t('New radios'),
      '#type' => 'radios',
      '#options' => array('one' => 'one', 'two' => 'two', 'three' => 'three'),
    ),
  );

  $fields['textfield'] = array(
    'title' => t('Textfield'),
    'properties' => array(
      'title',
      'description',
      'field_prefix',
      'field_suffix',
      'default_value',
      'required',
      'size',
    ),
    'default' => array(
      '#title' => t('New textfield'),
      '#type' => 'textfield',
    ),
  );

  $fields['textarea'] = array(
    'title' => t('Textarea'),
    'properties' => array(
      'title',
      'description',
      'default_value',
      'required',
      'rows',
      'cols',
      'input_format',
    ),
    'default' => array(
      '#title' => t('New textarea'),
      '#type' => 'textarea',
    ),
  );

  $fields['file'] = array(
    'title' => t('File'),
    'properties' => array(
      'title',
      'description',
      'required',
      'size',
      'file_extension_validate',
      'file_size_validate',
    ),
    'default' => array(
      '#title' => t('New file'),
      '#type' => 'file',
      '#size' => 30,
    ),
  );

  $fields['image'] = array(
    'title' => t('Image'),
    'properties' => array(
      'title',
      'description',
      'required',
    ),
    'validate' => array(
      'file_extension_validate',
      'file_size_validate',
      'file_image_size',
    ),
    'default' => array(
      '#title' => t('New image'),
      '#type' => 'file',
      '#size' => 30,
    ),
  );

  return array(
    'form_builder_examples' => $fields,
  );
}

/**
 * Implementation of hook_form_builder_preview_alter().
 */
function form_builder_examples_form_builder_preview_alter($element, $form_type, $form_id) {
  if ($form_type == 'form_builder_examples') {
    if (isset($element['#description'])) {
      $element['#description'] = filter_xss($element['#description']);
    }
  }
}

/**
 * Implementation of hook_form_builder_load().
 *
 * Convert a CCK content type into a standard form builder array.
 *
 * @param $form_id
 *   The unique identifier for the form being edited. For CCK, this is the
 *   content type name.
 */
function form_builder_examples_form_builder_load($form_id) {
  $form = form_builder_examples_example();

  // TODO: Convert content type information into a form builder array. Yuk.

  return $form;
}

/**
 * Make changes to the preview form to match the functionality of the form.
 *
 * This function is called during the "live preview" of the form. Use it to make
 * any adjustments to the FAPI structure that the final form with use.
 */
function form_builder_examples_form_builder_preview($form, $form_id) {
  foreach (element_children($form) as $key) {
    if (isset($form[$key]['#description'])) {
      $form[$key]['#description'] = filter_xss_admin($form[$key]['#description']);
    }
  }

  return $form;
}

/**
 * Implementation of hook_form_builder_save().
 *
 * Take a form builder array and save it to the content type.
 */
function form_builder_examples_form_builder_save($form_id, $form) {
  $type_name = $form_id;

  // TODO: Take form builder array and save it as a content type. Also yuk.
}

/**
 * Menu callback for importing an entire FAPI array.
 */
function form_builder_examples_import(&$form_state) {
  $form = array();

  $form['import'] = array(
    '#type' => 'textarea',
    '#title' => t('Import code'),
  );

  return $form;
}

/**
 * Menu callback for exporting an entire FAPI array.
 */
function form_builder_examples_export() {
  module_load_include('inc', 'form_builder', 'includes/form_builder.api');
  module_load_include('inc', 'form_builder', 'includes/form_builder.cache');

  $form = array();

  $current = form_builder_cache_load('form_builder_examples', 'sample');

  $form['export'] = array(
    '#type' => 'textarea',
    '#title' => t('Export code'),
    '#default_value' => form_builder_export($current),
    '#attributes' => array('readonly' => 'readonly', 'style' => 'font-family: monospace;'),
    '#rows' => 20,
  );

  return $form;
}