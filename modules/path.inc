<?php
// $Id$

/**
 * @file
 *   Form builder implementation for path.module.
 */

/**
 * Implementation of hook_form_builder_types().
 */
function path_form_builder_types() {
  $fields = array();

  // Make a default path settings to pass into the path form.
  $settings = array(
    'weight' => 30,
    'collapsible' => TRUE,
    'collapsed' => TRUE,
  );
  $default = path_node_form('', $settings);

  // Give the form item a pseudo field type.
  // This is then modified in path_form_builder_preview_alter().
  $default['path']['#key'] = 'path';
  $default['path']['#type'] = 'path_settings';

  $fields['path_settings'] = array(
    'title' => t('Path'),
    'properties' => array(
      'collapsible',
      'collapsed',
    ),
    'default' => $default['path'],
    'unique' => TRUE,
    'palette_group' => 'special',
  );

  return array(
    'node' => $fields,
    'example' => $fields,
  );
}

/**
 * Implementation of hook_form_builder_preview_alter().
 *
 * Convert the #type => path_settings pseudo-element into a real fieldset.
 */
function path_form_builder_preview_alter(&$element, $form_type, $form_id) {
  if ($element['#type'] == 'path_settings') {
    $element['#type'] = 'fieldset';
  }
}

/**
 * A straight copy/paste of path_form_alter(). Get the path node form.
 *
 * @param $path_settings
 *   An array with the following values:
 *   The contents of $node->path.
 *
 * @todo Put this function directly in path.module, abstracting it from
 * path_form_alter().
 */
function path_node_form($path, $path_settings) {
  $form = array();

  $form['path'] = array(
    '#type' => 'fieldset',
    '#title' => t('URL path settings'),
    '#collapsible' => $path_settings['collapsible'],
    '#collapsed' => $path_settings['collapsed'],
    '#access' => user_access('create url aliases'),
    '#weight' => $path_settings['weight'],
  );
  $form['path']['path'] = array(
    '#type' => 'textfield',
    '#default_value' => $path,
    '#maxlength' => 250,
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('Optionally specify an alternative URL by which this node can be accessed. For example, type "about" when writing an about page. Use a relative path and don\'t add a trailing slash or the URL alias won\'t work.'),
  );
  if ($path) {
    $form['path']['pid'] = array(
      '#type' => 'value',
      '#value' => db_result(db_query("SELECT pid FROM {url_alias} WHERE dst = '%s' AND language = '%s'", $path, $form['#node']->language))
    );
  }

  return $form;
}


/**
 * Implementation of hook_form_builder_load().
 */
function path_form_builder_load($form_type, $form_id) {
  if ($form_type == 'node') {
    $form['path']['#form_builder'] = array(
      'element_id' => 'path',
      'configurable' => TRUE,
      'removable' => TRUE,
      'unique' => TRUE,
    );
    $form['path']['#type'] = 'path_settings';

    return $form;
  }
}

/**
 * Implementation of hook_form_builder_save().
 */
function path_form_builder_save(&$form, $form_type, $form_id) {
  if ($form_type == 'node') {
    $element = form_builder_get_element($form, 'path');
    $settings = array(
      'collapsible' => $element['#collapsible'],
      'collapsed' => $element['#collapsed'],
      'weight' => $element['#weight'],
    );
    variable_set('path_settings_' . $form_type, $settings);
  }
}


/**
 * Implementation of hook_form_alter().
 *
 * @todo Merge this into path_form_alter().
 */
function path_form_builder_form_alter(&$form, &$form_state, $form_id) {
  // Set properties of the path form item based on the path node form settings.
  if (preg_match('/_node_form$/', $form_id) && isset($form['path'])) {
    $settings = array(
      'collapsible' => TRUE,
      'collapsed' => TRUE,
      'weight' => 30,
    );
    $settings = variable_get('path_settings_' . $form['#node']->type, $settings);
    $form['path']['#collapsible'] = $settings['collapsible'];
    $form['path']['#collapsed'] = $settings['collapsed'];
    $form['path']['#weight'] = $settings['weight'];
  }
}
