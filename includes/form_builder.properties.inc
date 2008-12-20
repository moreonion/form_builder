<?php
// $Id$

/**
 * @file form_builder.properties.inc
 * Implementations of hook_form_builder_properties in separate functions.
 */

/**
 * Configuration form for the "key" property.
 * 
 * The key property is special in that it's not actually part of the element,
 * but instead the array key that is used to reference the element in the
 * Form API structure.
 */
function form_builder_property_key_form(&$form_state, $form_type, $element) {
  $form = array();

  $form['key'] = array(
    '#title' => t('Form key'),
    '#type' => 'textfield',
    '#default_value' => $element['#key'],
    '#required' => TRUE,
    '#weight' => -9,
  );

  return $form;
}

/**
 * Configuration form for the "title" property.
 */
function form_builder_property_title_form(&$form_state, $form_type, $element) {
  $form = array();

  $form['title'] = array(
    '#title' => t('Title'),
    '#type' => 'textfield',
    '#default_value' => $element['#title'],
    '#required' => TRUE,
    '#weight' => -10,
  );

  return $form;
}


/**
 * Configuration form for the "weight" property.
 *
 * This field is in the "hidden" builder group, meaning it's never shown in
 * the main editing interface. However, it's still there if editing without JS.
 */
function form_builder_property_weight_form(&$form_state, $form_type, $element) {
  $form = array();

  if (!isset($_REQUEST['js'])) {
    $form['weight'] = array(
      '#form_builder' => array('property_group' => 'hidden'),
      '#type' => 'textfield',
      '#size' => 6,
      '#title' => t('Weight'),
      '#default_value' => $element['#weight'],
    );
  }

  return $form;
}

/**
 * Configuration form for the "description" property.
 */
function form_builder_property_description_form(&$form_state, $form_type, $element) {
  $form = array();

  $form['description'] = array(
    '#title' => t('Description'),
    '#type' => 'textarea',
    '#default_value' => $element['#description'],
    '#weight' => 5,
  );

  return $form;
}

/**
 * Configuration form for the "required" property.
 */
function form_builder_property_required_form(&$form_state, $form_type, $element) {
  $form = array();

  $form['required'] = array(
    '#form_builder' => array('property_group' => 'validation'),
    '#title' => t('Required'),
    '#type' => 'checkbox',
    '#default_value' => $element['#required'],
    '#weight' => -1,
  );

  return $form;
}

/**
 * Configuration form for the "options" property.
 */
function form_builder_property_options_form(&$form_state, $form_type, $element) {
  $form = array();

  // Checkboxes have an implied "multiple" property.
  if ($element['#type'] == 'checkboxes') {
    $element['#multiple'] = TRUE;
  }

  $form['options'] = array(
    '#form_builder' => array('property_group' => 'options'),
    '#title' => t('Options'),
    '#type' => 'options',
    '#default_value' => $element['#default_value'],
    '#options' => $element['#options'],
    '#required' => TRUE,
    '#multiple' => isset($element['#multiple']) ? $element['#multiple'] : FALSE,
    '#optgroups' => $element['#type'] == 'select' ? TRUE : FALSE,
  );

  // Remove the default value field, since it's handled by the options field.
  $form['default_value'] = array();

  return $form;
}

function form_builder_property_options_form_submit(&$form, &$form_state) {
  $options = $form_state['values']['options']['options'];
  $default_value = $form_state['values']['options']['default_value'];

  $form_state['values']['options'] = $options;
  $form_state['values']['default_value'] = $default_value;
}

/**
 * Configuration form for the "default_value" property.
 *
 * @todo Handle multiple value defaults, such as for checkboxes or selects.
 */
function form_builder_property_default_value_form(&$form_state, $form_type, $element) {
  $form = array();

  // If this default value is an array, convert it to a string.
  if (is_array($element['#default_value'])) {
    $element['#default_value'] = implode(', ', $element['#default_value']);
  }

  $form['default_value'] = array(
    '#type' => 'textfield',
    '#title' => t('Default value'),
    '#default_value' => $element['#default_value'],
    '#weight' => 1,
  );

  // Add help text if this is a multi-value field.
  if (!empty($element['#multiple'])) {
    $form['default_value']['#description'] = t('Multiple values may be specified by separating by a comma.');
  }

  return $form;
}


/**
 * Configuration form for the "multiple" property.
 */
function form_builder_property_multiple_form(&$form_state, $form_type, $element) {
  $form = array();

  $form['multiple'] = array(
    '#title' => t('Multiple'),
    '#type' => 'checkbox',
    '#default_value' => $element['#type'] != 'checkboxes' ? $element['#multiple'] : 1,
    '#weight' => 2,
    // This property is utilized by checkboxes, but no option is ever shown.
    '#access' => $element['#type'] != 'checkboxes',
  );

  return $form;
}

/**
 * Submit function for "multiple" property on checkboxes and select elements.
 */
function form_builder_property_multiple_form_submit(&$form, &$form_state) {
  // Convert default values to an array.
  if ($form_state['values']['multiple']) {
    // Expand on commas.
    $form_state['values']['default_value'] = explode(',', $form_state['values']['default_value']);
    // Trim space around options.
    foreach ($form_state['values']['default_value'] as $key => $value) {
      $form_state['values']['default_value'][$key] = trim($value);
    }
    // Remove any options with no value.
    $form_state['values']['default_value'] = array_filter($form_state['values']['default_value']);
  }
}

/**
 * Configuration form for the "size" property.
 */
function form_builder_property_size_form(&$form_state, $form_type, $element) {
  $form = array();

  $form['size'] = array(
    '#form_builder' => array('property_group' => 'display'),
    '#type' => 'textfield',
    '#size' => 6,
    '#title' => t('Size'),
    '#default_value' => $element['#size'],
    '#weight' => 2,
  );

  return $form;
}

/**
 * Configuration form for the "size" property.
 */
function form_builder_property_rows_form(&$form_state, $form_type, $element) {
  $form = array();

  $form['rows'] = array(
    '#form_builder' => array('property_group' => 'display'),
    '#type' => 'textfield',
    '#size' => 6,
    '#title' => t('Rows'),
    '#default_value' => $element['#rows'],
    '#weight' => 2,
  );

  return $form;
}

/**
 * Configuration form for the "cols" property.
 */
function form_builder_property_cols_form(&$form_state, $form_type, $element) {
  $form = array();

  $form['cols'] = array(
    '#form_builder' => array('property_group' => 'display'),
    '#type' => 'textfield',
    '#size' => 6,
    '#title' => t('Columns'),
    '#default_value' => $element['#cols'],
    '#weight' => 3,
    '#description' => t('The width of the textarea. This property might not have a visual impact depending on the CSS of your site.'),
  );

  return $form;
}


/**
 * Configuration form for the "field_prefix" property.
 */
function form_builder_property_field_prefix_form(&$form_state, $form_type, $element) {
  $form = array();

  $form['field_prefix'] = array(
    '#form_builder' => array('property_group' => 'display'),
    '#type' => 'textfield',
    '#title' => t('Prefix'),
    '#default_value' => $element['#field_prefix'],
    '#weight' => -2,
  );

  return $form;
}

/**
 * Configuration form for the "field_suffix" property.
 */
function form_builder_property_field_suffix_form(&$form_state, $form_type, $element) {
  $form = array();

  $form['field_suffix'] = array(
    '#form_builder' => array('property_group' => 'display'),
    '#type' => 'textfield',
    '#title' => t('Suffix'),
    '#default_value' => $element['#field_suffix'],
    '#weight' => -1,
  );

  return $form;
}

/**
 * Configuration form for the "collapsible" property.
 */
function form_builder_property_collapsible_form(&$form_state, $form_type, $element) {
  $form = array();

  $form['collapsible'] = array(
    '#form_builder' => array('property_group' => 'display'),
    '#type' => 'checkbox',
    '#title' => t('Collapsible'),
    '#default_value' => $element['#collapsible'],
    '#weight' => -2,
  );

  return $form;
}

/**
 * Configuration form for the "collapsed" property.
 */
function form_builder_property_collapsed_form(&$form_state, $form_type, $element) {
  $form = array();

  $form['collapsed'] = array(
    '#form_builder' => array('property_group' => 'display'),
    '#type' => 'checkbox',
    '#title' => t('Collapsed'),
    '#default_value' => $element['#collapsed'],
    '#weight' => -1,
  );

  return $form;
}