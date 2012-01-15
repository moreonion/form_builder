<?php

/**
 * @file
 *   Default webform component callbacks for functionality related to the Form Builder.
 */

/**
 * @defgroup form-builder-webform-date-callbacks Callbacks for the Date component
 * @{
 */

/**
 * Implements _form_builder_webform_form_builder_types_component().
 */
function _form_builder_webform_form_builder_types_date() {
  $fields = array();

  $fields['date'] = array(
    'title' => t('Date'),
    'properties' => array(
      'title',
      'description',
      'default_value',
      'required',
      'key',
      'title_display',
      // Date-specific properties.
      'timezone',
      'year_start',
      'year_end',
      'year_textfield',
      'datepicker',
    ),
  );
  $fields['date']['default'] = _form_builder_webform_default('date');
  $fields['date']['default']['#title'] = t('New date');

  return $fields;
}

/**
 * Implements _form_builder_webform_form_builder_properties_component().
 */
function _form_builder_webform_form_builder_properties_date() {
  return array(
    'timezone' => array(
      'form' => 'form_builder_webform_property_date_timezone_form',
    ),
    'year_start' => array(
      'form' => 'form_builder_webform_property_date_year_start_form',
    ),
    'year_end' => array(
      'form' => 'form_builder_webform_property_date_year_end_form',
    ),
    'year_textfield' => array(
      'form' => 'form_builder_webform_property_date_year_textfield_form',
    ),
    'datepicker' => array(
      'form' => 'form_builder_webform_property_date_datepicker_form',
    ),
  );
}

/**
 * Implements _form_builder_webform_form_builder_save_component().
 */
function _form_builder_webform_form_builder_save_date($component, $form_element) {
  $component['extra']['description'] = isset($form_element['#description']) ? $form_element['#description'] : NULL;
  $component['extra']['disabled'] = isset($form_element['#disabled']) ? $form_element['#disabled'] : FALSE;
  $component['extra']['title_display'] = isset($form_element['#title_display']) ? $form_element['#title_display'] : 'before';
  $component['extra']['timezone'] = isset($form_element['#timezone']) ? $form_element['#timezone'] : 'user';
  $component['extra']['year_start'] = isset($form_element['#year_start']) ? $form_element['#year_start'] : '-2';
  $component['extra']['year_end'] = isset($form_element['#year_end']) ? $form_element['#year_end'] : '+2';
  $component['extra']['year_textfield'] = isset($form_element['#year_textfield']) ? $form_element['#year_textfield'] : FALSE;
  $component['extra']['datepicker'] = isset($form_element['#datepicker']) ? $form_element['#datepicker'] : FALSE;
  return $component;
}

/**
 * Configuration form for the "timezone" property.
 */
function form_builder_webform_property_date_timezone_form(&$form_state, $form_type, $element) {
  return _form_builder_webform_build_edit_form('date', $element, 'timezone', NULL, array('extra', 'timezone'), array('extra', 'timezone'));
}

/**
 * Configuration form for the "year_start" property.
 */
function form_builder_webform_property_date_year_start_form(&$form_state, $form_type, $element) {
  return _form_builder_webform_build_edit_form('date', $element, 'year_start', 'validation', array('validation', 'year_start'), array('extra', 'year_start'));
}

/**
 * Configuration form for the "year_end" property.
 */
function form_builder_webform_property_date_year_end_form(&$form_state, $form_type, $element) {
  return _form_builder_webform_build_edit_form('date', $element, 'year_end', 'validation', array('validation', 'year_end'), array('extra', 'year_end'));
}

/**
 * Configuration form for the "year_textfield" property.
 */
function form_builder_webform_property_date_year_textfield_form(&$form_state, $form_type, $element) {
  return _form_builder_webform_build_edit_form('date', $element, 'year_textfield', 'display', array('display', 'year_textfield'), array('extra', 'year_textfield'));
}

/**
 * Configuration form for the "datepicker" property.
 */
function form_builder_webform_property_date_datepicker_form(&$form_state, $form_type, $element) {
  return _form_builder_webform_build_edit_form('date', $element, 'datepicker', 'display', array('display', 'datepicker'), array('extra', 'datepicker'));
}

/**
 * @} End of "defgroup form-builder-webform-date-callbacks"
 */

/**
 * @defgroup form-builder-webform-email-callbacks Callbacks for the Email component
 * @{
 */

/**
 * Implements _form_builder_webform_form_builder_types_component().
 */
function _form_builder_webform_form_builder_types_email() {
  $fields = array();

  $fields['email'] = array(
    'title' => t('E-mail'),
    'properties' => array(
      'title',
      'description',
      'default_value',
      'required',
      'size',
      'key',
      'title_display',
    ),
  );
  $fields['email']['default'] = _form_builder_webform_default('email');
  $fields['email']['default'] = _form_builder_webform_form_builder_load_email($fields['email']['default']);

  $fields['email']['default']['#title'] = t('New e-mail');

  return $fields;
}

/**
 * Implements _form_builder_webform_form_builder_load_component().
 */
function _form_builder_webform_form_builder_load_email($form_element) {
  if (isset($form_element['#webform_component']['type']) && $form_element['#webform_component']['type'] == 'email') {
    // Elements of type "webform_email" are known to Form Builder as simply an
    // "email" field. This makes it so that we use the generic email icon.
    $form_element['#form_builder']['element_type'] = 'email';
  }
  return $form_element;
}

/**
 * Implements _form_builder_webform_form_builder_save_component().
 */
function _form_builder_webform_form_builder_save_email($component, $form_element) {
  $component['extra']['width'] = isset($form_element['#size']) ? $form_element['#size'] : NULL;
  $component['extra']['description'] = isset($form_element['#description']) ? $form_element['#description'] : NULL;
  $component['extra']['disabled'] = isset($form_element['#disabled']) ? $form_element['#disabled'] : FALSE;
  $component['extra']['title_display'] = isset($form_element['#title_display']) ? $form_element['#title_display'] : 'before';
  return $component;
}

/**
 * @} End of "defgroup form-builder-webform-email-callbacks"
 */

/**
 * @defgroup form-builder-webform-fieldset-callbacks Callbacks for the Fieldset component
 * @{
 */

/**
 * Implements _form_builder_webform_form_builder_types_component().
 */
function _form_builder_webform_form_builder_types_fieldset() {
  $fields = array();

  $fields['fieldset'] = array(
    'title' => t('Fieldset'),
    'properties' => array(
      'title',
      'description',
      'collapsible',
      'collapsed',
      'key',
    ),
  );
  $fields['fieldset']['default'] = _form_builder_webform_default('fieldset');
  $fields['fieldset']['default']['#title'] = t('New fieldset');

  return $fields;
}

/**
 * Implements _form_builder_webform_form_builder_save_component().
 */
function _form_builder_webform_form_builder_save_fieldset($component, $form_element) {
  $component['extra']['collapsible'] = isset($form_element['#collapsible']) ? $form_element['#collapsible'] : NULL;
  $component['extra']['collapsed'] = isset($form_element['#collapsed']) ? $form_element['#collapsed'] : NULL;
  $component['extra']['description'] = isset($form_element['#description']) ? $form_element['#description'] : '';
  return $component;
}

/**
 * @} End of "defgroup form-builder-webform-fieldset-callbacks"
 */

/**
 * @defgroup form-builder-webform-file-callbacks Callbacks for the File component
 * @{
 */

/**
 * Implements _form_builder_webform_form_builder_types_component().
 */
function _form_builder_webform_form_builder_types_file() {
  $fields = array();

  $fields['file'] = array(
    'title' => t('File'),
    'properties' => array(
      'title',
      'description',
      'required',
      'key',
      'title_display',
      'webform_file_filtering',
      'webform_file_scheme',
      'webform_file_directory',
      'webform_file_progress_indicator',
      // TODO: The manged_file element in core doesn't support #size (or width).
      //'webform_file_width',
    ),
  );

  $fields['file']['default'] = _form_builder_webform_default('file');
  $fields['file']['default'] = _form_builder_webform_form_builder_load_file($fields['file']['default']);
  $fields['file']['default']['#title'] = t('New file');

  return $fields;
}

/**
 * Implements _form_builder_webform_form_builder_properties_component().
 */
function _form_builder_webform_form_builder_properties_file() {
  return array(
    'webform_file_filtering' => array(
      'form' => 'form_builder_webform_property_webform_file_filtering_form',
    ),
    'webform_file_scheme' => array(
      'form' => 'form_builder_webform_property_webform_file_scheme_form',
    ),
    'webform_file_directory' => array(
      'form' => 'form_builder_webform_property_webform_file_directory_form',
    ),
    'webform_file_progress_indicator' => array(
      'form' => 'form_builder_webform_property_webform_file_progress_indicator_form',
    ),
    'webform_file_width' => array(
      'form' => 'form_builder_webform_property_webform_file_width_form',
    ),
  );
}

/**
 * Implements _form_builder_webform_form_builder_load_component().
 */
function _form_builder_webform_form_builder_load_file($form_element) {
  // If we are loading a file element, _webform_render_file() adds some extra
  // structure (including several element children) that we don't want here.
  // Instead, we need the element itself to represent the file component
  // directly.
  if (isset($form_element['#webform_component']['type']) && $form_element['#webform_component']['type'] == 'file') {
    // Elements of type "managed_file" are known to Form Builder as simply an
    // "file" field. This makes it so that we use the generic file icon.
    $form_element['#form_builder']['element_type'] = 'file';

    // Set the custom webform file properties based on the values stored with
    // the webform component.
    if (isset($form_element['#webform_component']['extra']['filtering'])) {
      $form_element['#webform_file_filtering'] = $form_element['#webform_component']['extra']['filtering'];
    }
    if (isset($form_element['#webform_component']['extra']['scheme'])) {
      $form_element['#webform_file_scheme'] = $form_element['#webform_component']['extra']['scheme'];
    }
    if (isset($form_element['#webform_component']['extra']['directory'])) {
      $form_element['#webform_file_directory'] = $form_element['#webform_component']['extra']['directory'];
    }
    if (isset($form_element['#webform_component']['extra']['progress_indicator'])) {
      $form_element['#webform_file_progress_indicator'] = $form_element['#webform_component']['extra']['progress_indicator'];
    }
    if (isset($form_element['#webform_component']['extra']['width'])) {
      $form_element['#webform_file_width'] = $form_element['#webform_component']['extra']['width'];
    }
  }

  return $form_element;
}

/**
 * Implements _form_builder_webform_form_builder_save_component().
 */
function _form_builder_webform_form_builder_save_file($component, $form_element) {
  $component['extra']['description'] = isset($form_element['#description']) ? $form_element['#description'] : NULL;
  $component['extra']['disabled'] = isset($form_element['#disabled']) ? $form_element['#disabled'] : FALSE;
  $component['extra']['title_display'] = isset($form_element['#title_display']) ? $form_element['#title_display'] : 'before';
  $component['extra']['filtering'] = isset($form_element['#webform_file_filtering']) ? $form_element['#webform_file_filtering'] : array();
  $component['extra']['scheme'] = isset($form_element['#webform_file_scheme']) ? $form_element['#webform_file_scheme'] : NULL;
  $component['extra']['directory'] = isset($form_element['#webform_file_directory']) ? $form_element['#webform_file_directory'] : NULL;
  $component['extra']['progress_indicator'] = isset($form_element['#webform_file_progress_indicator']) ? $form_element['#webform_file_progress_indicator'] : NULL;
  $component['extra']['width'] = isset($form_element['#webform_file_width']) ? $form_element['#webform_file_width'] : NULL;

  return $component;
}

/**
 * Configuration form for the "webform_file_filtering" property.
 */
function form_builder_webform_property_webform_file_filtering_form(&$form_state, $form_type, $element) {
  return _form_builder_webform_build_edit_form('file', $element, 'webform_file_filtering', 'validation', array('validation', 'filtering'), array('extra', 'filtering'));
}

/**
 * Configuration form for the "webform_file_scheme" property.
 */
function form_builder_webform_property_webform_file_scheme_form(&$form_state, $form_type, $element) {
  return _form_builder_webform_build_edit_form('file', $element, 'webform_file_scheme', 'default', array('extra', 'scheme'), array('extra', 'scheme'));
}

/**
 * Configuration form for the "webform_file_directory" property.
 */
function form_builder_webform_property_webform_file_directory_form(&$form_state, $form_type, $element) {
  return _form_builder_webform_build_edit_form('file', $element, 'webform_file_directory', 'default', array('extra', 'directory'), array('extra', 'directory'));
}

/**
 * Configuration form for the "webform_file_progress_indicator" property.
 */
function form_builder_webform_property_webform_file_progress_indicator_form(&$form_state, $form_type, $element) {
  return _form_builder_webform_build_edit_form('file', $element, 'webform_file_progress_indicator', 'display', array('display', 'progress_indicator'), array('extra', 'progress_indicator'));
}

/**
 * Configuration form for the "webform_file_width" property.
 */
function form_builder_webform_property_webform_file_width_form(&$form_state, $form_type, $element) {
  return _form_builder_webform_build_edit_form('file', $element, 'webform_file_width', 'display', array('display', 'width'), array('extra', 'width'));
}

/**
 * @} End of "defgroup form-builder-webform-file-callbacks"
 */

/**
 * @defgroup form-builder-webform-grid-callbacks Callbacks for the Grid component
 * @{
 */

/**
 * Implements _form_builder_webform_form_builder_types_component().
 */
function _form_builder_webform_form_builder_types_grid() {
  $fields = array();

  $fields['grid'] = array(
    'title' => t('Grid'),
    'properties' => array(
      'title',
      'description',
      'required',
      'key',
      'title_display',
      // Grid-specific properties.
      'grid_options',
      'grid_questions',
      'custom_grid_option_keys',
      'custom_grid_question_keys',
      'optrand',
      'qrand',
    ),
  );
  $fields['grid']['default'] = _form_builder_webform_default('grid');
  $fields['grid']['default']['#title'] = t('New grid');
  $fields['grid']['default']['#grid_options'] = array(t('poor'), t('average'), t('good'), t('excellent'));
  $fields['grid']['default']['#grid_questions'] = array(t('question one'), t('question two'), t('question three'));

  return $fields;
}

/**
 * Implements _form_builder_webform_form_builder_properties_component().
 */
function _form_builder_webform_form_builder_properties_grid() {
  return array(
    'grid_options' => array(
      'form' => 'form_builder_webform_property_grid_options_form',
      'submit' => array('form_builder_webform_property_grid_form_submit'),
    ),
    'grid_questions' => array(
      'form' => 'form_builder_webform_property_grid_questions_form',
      'submit' => array('form_builder_webform_property_grid_form_submit'),
    ),
    'optrand' => array(
      'form' => 'form_builder_webform_property_grid_optrand_form'
    ),
    'qrand' => array(
      'form' => 'form_builder_webform_property_grid_qrand_form'
    ),
  );
}

/**
 * Implements _form_builder_webform_form_builder_load_component().
 */
function _form_builder_webform_form_builder_load_grid($element) {
  if (isset($element['#type']) && $element['#type'] == 'webform_grid') {
    // Elements of type "webform_grid" are known to Form Builder as simply a
    // "grid" field. This makes it so that we use the generic grid icon.
    $element['#form_builder']['element_type'] = 'grid';

    // Convert properties used only in Form Builder.
    $element['#custom_grid_option_keys'] = !empty($element['#webform_component']['extra']['custom_option_keys']);
    $element['#custom_grid_question_keys'] = !empty($element['#webform_component']['extra']['custom_question_keys']);
  }
  return $element;
}

/**
 * Implements _form_builder_webform_form_builder_save_component().
 */
function _form_builder_webform_form_builder_save_grid($component, $form_element) {
  $options = '';

  foreach ($form_element['#grid_options'] as $key => $option_value) {
    $options .= $key . '|' . $option_value . "\n";
  }

  $questions = '';
  foreach ($form_element['#grid_questions'] as $key => $question_value) {
    $questions .= $key . '|' . $question_value . "\n";
  }

  $component['extra']['description'] = isset($form_element['#description']) ? $form_element['#description'] : NULL;
  $component['extra']['title_display'] = isset($form_element['#title_display']) ? $form_element['#title_display'] : 'before';
  $component['extra']['options'] = $options;
  $component['extra']['questions'] = $questions;
  $component['extra']['custom_option_keys'] = isset($form_element['#custom_grid_option_keys']) ? $form_element['#custom_grid_option_keys'] : FALSE;
  $component['extra']['custom_question_keys'] =  isset($form_element['#custom_grid_question_keys']) ? $form_element['#custom_grid_question_keys'] : FALSE;
  $component['extra']['optrand'] = !empty($form_element['#optrand']);
  $component['extra']['qrand'] = !empty($form_element['#qrand']);

  return $component;
}

/**
 * Configuration form for the "grid_options" property.
 */
function form_builder_webform_property_grid_options_form(&$form_state, $form_type, $element) {
  $element['#grid_options'] = _webform_select_options_to_text($element['#grid_options']);
  $form = _form_builder_webform_build_edit_form('grid', $element, 'grid_options', 'options', array('options'), array('extra', 'options'));
  // The _form_builder_webform_build_edit_form() function can only pull in one
  // property at a time, so we have to set the key toggle manually.
  $form['grid_options']['options']['#key_type_toggled'] = !empty($element['#custom_grid_option_keys']);
  return $form;
}

/**
 * Configuration form for the "grid_questions" property.
 */
function form_builder_webform_property_grid_questions_form(&$form_state, $form_type, $element) {
  $element['#grid_questions'] = _webform_select_options_to_text($element['#grid_questions']);
  $form = _form_builder_webform_build_edit_form('grid', $element, 'grid_questions', 'options', array('questions'), array('extra', 'questions'));
  // The _form_builder_webform_build_edit_form() function can only pull in one
  // property at a time, so we have to set the key toggle manually.
  $form['grid_questions']['options']['#key_type_toggled'] = !empty($element['#custom_grid_question_keys']);
  return $form;
}

/**
 * Submit handler for the "grid_options" and "grid_questions" properties.
 */
function form_builder_webform_property_grid_form_submit(&$form, &$form_state) {
  if (empty($form_state['values']['grid_options'])) {
    $form_state['values']['grid_options'] = _webform_select_options_from_text($form_state['values']['extra']['grid_options']);
    $form_state['values']['custom_grid_option_keys'] = $form_state['values']['extra']['custom_grid_option_keys'];
  }

  if (empty($form_state['values']['grid_questions'])) {
    $form_state['values']['grid_questions'] = _webform_select_options_from_text($form_state['values']['extra']['grid_questions']);
    $form_state['values']['custom_grid_question_keys'] = $form_state['values']['extra']['custom_grid_question_keys'];
  }
}

/**
 * Configuration form for the "optrand" property.
 */
function form_builder_webform_property_grid_optrand_form(&$form_state, $form_type, $element) {
  return _form_builder_webform_build_edit_form('grid', $element, 'optrand', 'display', array('display', 'optrand'), array('extra', 'optrand'));
}

/**
 * Configuration form for the "qrand" property.
 */
function form_builder_webform_property_grid_qrand_form(&$form_state, $form_type, $element) {
  return _form_builder_webform_build_edit_form('grid', $element, 'qrand', 'display', array('display', 'qrand'), array('extra', 'qrand'));
}

/**
 * @} End of "defgroup form-builder-webform-time-callbacks"
 */

/**
 * @defgroup form-builder-webform-hidden-callbacks Callbacks for the Hidden component
 * @{
 */

/**
 * Implements _form_builder_webform_form_builder_types_component().
 */
function _form_builder_webform_form_builder_types_hidden() {
  $fields = array();

  $fields['hidden'] = array(
    'title' => t('Hidden'),
    'properties' => array(
      'title',
      'default_value',
      'key',
    ),
  );
  $fields['hidden']['default'] = _form_builder_webform_default('hidden');
  $fields['hidden']['default']['#title'] = t('New hidden');

  return $fields;
}

/**
 * Implements _form_builder_webform_form_builder_preview_alter_component().
 */
function _form_builder_webform_form_builder_preview_alter_hidden($form_element) {
  // Convert the hidden element to a markup element before displaying it.
  $form_element['#type'] = 'markup';
  $form_element['#form_builder']['element_type'] = 'markup';
  unset($form_element['#theme']);

  // Add the 'webform_element' theme wrapper at the beginning, where it would
  // normally be.
  array_unshift($form_element['#theme_wrappers'], 'webform_element');

  // Display the title of the hidden field as regular markup.
  $form_element['#children'] = t('@title - <em>hidden field</em>', array('@title' => $form_element['#title']));
  $form_element['#title_display'] = 'none';

  // Give the element a wrapper class so that themers can recognize it
  // represents a hidden element.
  $form_element['#attributes']['class'][] = 'form-builder-preview-hidden-webform-element';
  array_unshift($form_element['#theme_wrappers'], 'container');

  return $form_element;
}

/**
 * @} End of "defgroup form-builder-webform-hidden-callbacks"
 */

/**
 * @defgroup form-builder-webform-markup-callbacks Callbacks for the Markup component
 * @{
 */

/**
 * Implements _form_builder_webform_form_builder_types_component().
 */
function _form_builder_webform_form_builder_types_markup() {
  $fields = array();

  $default = '';
  $default .= '<p><strong>' . t('New HTML Markup') . '</strong></p>' . "\n\n";
  $default .= '<p>' . t('Enter any text supported by available text formats.') . '</p>';

  $fields['markup'] = array(
    'title' => t('Markup'),
    'properties' => array(
      'title',
      'markup',
      'format',
      'key',
    ),
  );
  $fields['markup']['default'] = _form_builder_webform_default('markup');
  $fields['markup']['default']['#markup'] = $default;
  $fields['markup']['default']['#title'] = t('New markup');

  return $fields;
}

/**
 * Implements _form_builder_webform_form_builder_save_component().
 */
function _form_builder_webform_form_builder_save_markup($component, $form_element) {
  $component['value'] = $form_element['#markup'];
  $component['extra']['format'] = isset($form_element['#format']) ? $form_element['#format'] : filter_default_format();
  $component['extra']['title_display'] = isset($form_element['#title_display']) ? $form_element['#title_display'] : 'before';
  return $component;
}

/**
 * Implements _form_builder_webform_form_builder_preview_alter_component().
 */
function _form_builder_webform_form_builder_preview_alter_markup($form_element) {
  // Filter the markup with the appropriate text format before displaying it.
  $form_element['#markup'] = isset($form_element['#markup']) ? $form_element['#markup'] : '';
  $format = isset($form_element['#format']) ? $form_element['#format'] : filter_default_format();
  $form_element['#markup'] = check_markup(_webform_filter_values($form_element['#markup'], NULL, NULL, NULL, FALSE), $format, '', TRUE);
  if (strlen(trim($form_element['#markup'])) == 0) {
    $form_element['#markup'] = t('Empty markup field');
  }

  // This runs after drupal_pre_render_markup so the changed markup 
  // needs to be copied to #children.
  $form_element['#children'] = $form_element['#markup'];

  // Do not show the title, since it will not display in the final webform
  // either.
  $form_element['#title_display'] = 'none';

  return $form_element;
}

/**
 * @} End of "defgroup form-builder-webform-markup-callbacks"
 */


/**
 * @defgroup form-builder-webform-number-callbacks Callbacks for the Number component
 * @{
 */

/**
 * Implements _form_builder_webform_form_builder_types_component().
 */
function _form_builder_webform_form_builder_types_number() {
  $fields = array();

  $fields['number'] = array(
    'title' => t('Number'),
    'properties' => array(
      'title',
      'description',
      'required',
      'key',
      'title_display',
      'default_value',
      'field_prefix',
      'field_suffix',
      // Number-specific properties.
      // TODO: Implement the number type support when possible through Webform.
      //'number_type',
      'min',
      'max',
      'step',
      'decimals',
      'point',
      'separator',
      'integer',
      'excludezero',
    ),
  );

  $fields['number']['default'] = _form_builder_webform_default('number');
  $fields['number']['default']['#title'] = t('New number');

  return $fields;
}

/**
 * Implements _form_builder_webform_form_builder_map_component().
 */
function _form_builder_webform_form_builder_map_number() {
  return array(
    'form_builder_type' => 'number',
    'properties' => array(
      'maxlength' => array(
        'storage_parents' => array('extra', 'maxlength'),
      ),
      'field_prefix' => array(
        'storage_parents' => array('extra', 'field_prefix'),
      ),
      'field_suffix' => array(
        'storage_parents' => array('extra', 'field_suffix'),
      ),
      'number_type' => array(
        'form_parents' => array('display', 'type'),
        'storage_parents' => array('extra', 'type'),
      ),
      'min' => array(
        'form_parents' => array('validation', 'min'),
        'storage_parents' => array('extra', 'min'),
      ),
      'max' => array(
        'form_parents' => array('validation', 'max'),
        'storage_parents' => array('extra', 'max'),
      ),
      'step' => array(
        'form_parents' => array('validation', 'step'),
        'storage_parents' => array('extra', 'step'),
      ),
      'decimals' => array(
        'form_parents' => array('display', 'decimals'),
        'storage_parents' => array('extra', 'decimals'),
      ),
      'point' => array(
        'form_parents' => array('display', 'point'),
        'storage_parents' => array('extra', 'point'),
      ),
      'separator' => array(
        'form_parents' => array('display', 'separator'),
        'storage_parents' => array('extra', 'separator'),
      ),
      'integer' => array(
        'form_parents' => array('validation', 'integer'),
        'storage_parents' => array('extra', 'integer'),
      ),
      'excludezero' => array(
        'form_parents' => array('analysis', 'excludezero'),
        'storage_parents' => array('extra', 'excludezero'),
      ),
    ),
  );
}

/**
 * @} End of "defgroup form-builder-webform-number-callbacks"
 */

/**
 * @defgroup form-builder-webform-pagebreak-callbacks Callbacks for the Pagebreak component
 * @{
 */

/**
 * Implements _form_builder_webform_form_builder_types_component().
 */
function _form_builder_webform_form_builder_types_pagebreak() {
  $fields = array();

  $fields['pagebreak'] = array(
    'title' => t('Page break'),
    'properties' => array(
      'title',
      'key',
    ),
    'default' => array(
      '#title' => t('Page break'),
      '#type' => 'pagebreak',
    ),
  );

  return $fields;
}

/**
 * Implements _form_builder_webform_form_builder_load_component().
 */
function _form_builder_webform_form_builder_load_pagebreak($form_element) {
  // Pagebreak components are rendered as hidden elements by webform, but
  // hidden elements do not have a #title property. So we have to convert the
  // rendered value to be used as the #title instead.
  if ($form_element['#type'] == 'pagebreak') {
    $form_element['#title'] = $form_element['#value'];
  }
  return $form_element;
}

/**
 * Implements _form_builder_webform_form_builder_preview_alter_component().
 */
function _form_builder_webform_form_builder_preview_alter_pagebreak($form_element) {
  // Convert the pagebreak element to a markup element before displaying it.
  $form_element['#type'] = 'markup';
  $form_element['#form_builder']['element_type'] = 'markup';
  unset($form_element['#theme']);

  // Add the 'webform_element' theme wrapper at the beginning, where it would
  // normally be.
  array_unshift($form_element['#theme_wrappers'], 'webform_element');

  // Display the title of the pagebreak field as regular markup.
  $form_element['#children'] = '--- ' . check_plain($form_element['#title']) . ' ---';
  $form_element['#children'] .= '<div class="description">' . t('(Fields below will be displayed on a new page)') . '</div>';
  $form_element['#title_display'] = 'none';

  // Give the element a wrapper class so that themers can recognize it
  // represents a pagebreak element.
  $form_element['#attributes']['class'][] = 'form-builder-preview-pagebreak-webform-element';
  array_unshift($form_element['#theme_wrappers'], 'container');

  return $form_element;
}

/**
 * @} End of "defgroup form-builder-webform-pagebreak-callbacks"
 */

/**
 * @defgroup form-builder-webform-select-callbacks Callbacks for the Select component
 * @{
 */

/**
 * Implements _form_builder_webform_form_builder_types_component().
 */
function _form_builder_webform_form_builder_types_select() {
  $fields = array();

  $fields['select'] = array(
    'title' => t('Select list'),
    'properties' => array(
      'title',
      'description',
      'default_value',
      'required',
      'options',
      'multiple',
      'key',
      'title_display',
    ),
  );
  $fields['select']['default'] = _form_builder_webform_default('select', array('aslist' => TRUE, 'multiple' => FALSE));
  $fields['select']['default']['#options'] = array(t('one'), t('two'), t('three'));
  $fields['select']['default']['#default_value'] = 0;
  $fields['select']['default']['#title'] = t('New select');
  $fields['select']['default']['#webform'] = array('type' => 'select');

  $fields['checkboxes'] = array(
    'title' => t('Checkboxes'),
    'properties' => array(
      'title',
      'description',
      'default_value',
      'required',
      'options',
      'multiple',
      'key',
      'title_display',
    ),
  );
  $fields['checkboxes']['default'] = _form_builder_webform_default('select', array('aslist' => FALSE, 'multiple' => TRUE));
  $fields['checkboxes']['default']['#options'] = array(t('one'), t('two'), t('three'));
  $fields['checkboxes']['default']['#default_value'] = array(0, 1);
  $fields['checkboxes']['default']['#title'] = t('New checkboxes');
  $fields['checkboxes']['default']['#webform'] = array('type' => 'select');

  $fields['radios'] = array(
    'title' => t('Radios'),
    'properties' => array(
      'title',
      'description',
      'default_value',
      'required',
      'options',
      'key',
      'title_display',
    ),
  );
  $fields['radios']['default'] = _form_builder_webform_default('select', array('aslist' => FALSE, 'multiple' => FALSE));
  $fields['radios']['default']['#options'] = array(t('one'), t('two'), t('three'));
  $fields['radios']['default']['#default_value'] = 0;
  $fields['radios']['default']['#title'] = t('New radios');
  $fields['radios']['default']['#webform'] = array('type' => 'select');

  return $fields;
}

/**
 * Implements _form_builder_webform_form_builder_load_component().
 */
function _form_builder_webform_form_builder_load_select($form_element) {
  if (in_array($form_element['#type'], array('radios', 'checkboxes', 'select'))) {
    $form_element['#webform']['type'] = 'select';
    if ($form_element['#type'] == 'checkboxes') {
      $form_element['#multiple'] = TRUE;
    }
    return $form_element;
  }
}

/**
 * Implements _form_builder_webform_form_builder_save_component().
 */
function _form_builder_webform_form_builder_save_select($component, $form_element) {
  $options = '';

  foreach ($form_element['#options'] as $key => $option_value) {
    $options .= $key . '|' . $option_value . "\n";
  }

  $component['extra']['items'] = $options;
  $component['extra']['multiple'] = $form_element['#form_builder']['element_type'] == 'checkboxes';
  $component['extra']['aslist'] = $form_element['#form_builder']['element_type'] == 'select';
  $component['extra']['description'] = isset($form_element['#description']) ? $form_element['#description'] : '';
  $component['extra']['title_display'] = isset($form_element['#title_display']) ? $form_element['#title_display'] : 'before';
  return $component;
}

/**
 * @} End of "defgroup form-builder-webform-select-callbacks"
 */

/**
 * @defgroup form-builder-webform-textarea-callbacks Callbacks for the Textarea component
 * @{
 */

/**
 * Implements _form_builder_webform_form_builder_types_component().
 */
function _form_builder_webform_form_builder_types_textarea() {
  $fields = array();

  $fields['textarea'] = array(
    'title' => t('Textarea'),
    'properties' => array(
      'title',
      'description',
      'default_value',
      'required',
      'rows',
      'cols',
      'key',
      'title_display',
    ),
  );
  $fields['textarea']['default'] = _form_builder_webform_default('textarea');
  $fields['textarea']['default']['#title'] = t('New textarea');

  return $fields;
}

/**
 * Implements _form_builder_webform_form_builder_save_component().
 */
function _form_builder_webform_form_builder_save_textarea($component, $form_element) {
  $component['extra']['description'] = $form_element['#description'];
  $component['extra']['cols'] = $form_element['#cols'];
  $component['extra']['rows'] = $form_element['#rows'];
  $component['extra']['disabled'] = empty($form_element['#disabled']) ? NULL : TRUE;
  $component['extra']['title_display'] = isset($form_element['#title_display']) ? $form_element['#title_display'] : 'before';
  return $component;
}

/**
 * @} End of "defgroup form-builder-webform-textarea-callbacks"
 */

/**
 * @defgroup form-builder-webform-textfield-callbacks Callbacks for the Textfield component
 * @{
 */

/**
 * Implements _form_builder_webform_form_builder_types_component().
 */
function _form_builder_webform_form_builder_types_textfield() {
  $fields = array();

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
      'key',
      'title_display',
    ),
  );
  $fields['textfield']['default'] = _form_builder_webform_default('textfield');
  $fields['textfield']['default']['#title'] = t('New textfield');

  return $fields;
}

/**
 * Implements _form_builder_webform_form_builder_save_component().
 */
function _form_builder_webform_form_builder_save_textfield($component, $form_element) {
  $component['extra']['width'] = isset($form_element['#size']) ? $form_element['#size'] : NULL;
  $component['extra']['maxlength'] = isset($form_element['#maxlength']) ? $form_element['#maxlength'] : NULL;
  $component['extra']['field_prefix'] = isset($form_element['#field_prefix']) ? $form_element['#field_prefix'] : NULL;
  $component['extra']['field_suffix'] = isset($form_element['#field_suffix']) ? $form_element['#field_suffix'] : NULL;
  $component['extra']['description'] = isset($form_element['#description']) ? $form_element['#description'] : NULL;
  $component['extra']['disabled'] = isset($form_element['#disabled']) ? $form_element['#disabled'] : FALSE;
  $component['extra']['title_display'] = isset($form_element['#title_display']) ? $form_element['#title_display'] : 'before';
  return $component;
}

/**
 * @} End of "defgroup form-builder-webform-textfield-callbacks"
 */

/**
 * @defgroup form-builder-webform-time-callbacks Callbacks for the Time component
 * @{
 */

/**
 * Implements _form_builder_webform_form_builder_types_component().
 */
function _form_builder_webform_form_builder_types_time() {
  $fields = array();

  $fields['time'] = array(
    'title' => t('Time'),
    'properties' => array(
      'title',
      'description',
      'default_value',
      'required',
      'key',
      'title_display',
      // Date-specific properties.
      'timezone',
      'hourformat',
    ),
  );
  $fields['time']['default'] = _form_builder_webform_default('time');
  $fields['time']['default']['#title'] = t('New time');

  return $fields;
}

/**
 * Implements _form_builder_webform_form_builder_properties_component().
 */
function _form_builder_webform_form_builder_properties_time() {
  return array(
    'timezone' => array(
      'form' => 'form_builder_webform_property_time_timezone_form',
    ),
    'hourformat' => array(
      'form' => 'form_builder_webform_property_time_hourformat_form',
    ),
  );
}

/**
 * Implements _form_builder_webform_form_builder_load_component().
 */
function _form_builder_webform_form_builder_load_time($element) {
  if (isset($element['#type']) && $element['#type'] == 'webform_time') {
    // Elements of type "webform_time" are known to Form Builder as simply a
    // "time" field. This makes it so that we use the generic time icon.
    $element['#form_builder']['element_type'] = 'time';
  }
  return $element;
}

/**
 * Implements _form_builder_webform_form_builder_save_component().
 */
function _form_builder_webform_form_builder_save_time($component, $form_element) {
  $component['extra']['description'] = isset($form_element['#description']) ? $form_element['#description'] : NULL;
  $component['extra']['disabled'] = isset($form_element['#disabled']) ? $form_element['#disabled'] : FALSE;
  $component['extra']['title_display'] = isset($form_element['#title_display']) ? $form_element['#title_display'] : 'before';
  $component['extra']['timezone'] = isset($form_element['#timezone']) ? $form_element['#timezone'] : 'user';
  $component['extra']['hourformat'] = isset($form_element['#hourformat']) ? $form_element['#hourformat'] : '12-hour';
  return $component;
}

/**
 * Configuration form for the "timezone" property.
 */
function form_builder_webform_property_time_timezone_form(&$form_state, $form_type, $element) {
  return _form_builder_webform_build_edit_form('time', $element, 'timezone', NULL, array('extra', 'timezone'), array('extra', 'timezone'));
}

/**
 * Configuration form for the "hourformat" property.
 */
function form_builder_webform_property_time_hourformat_form(&$form_state, $form_type, $element) {
  return _form_builder_webform_build_edit_form('time', $element, 'hourformat', 'display', array('display', 'hourformat'), array('extra', 'hourformat'));
}

/**
 * @} End of "defgroup form-builder-webform-time-callbacks"
 */

/**
 * Helper function; Generate a configuration form based on a map.
 */
function _form_builder_webform_mapped_form(&$form_state, $form_type, $element, $property) {
  $form = array();
  if ($component_type = $element['#webform_component']['type']) {
    if ($map = form_builder_webform_component_invoke($component_type, 'form_builder_map')) {
      $property_group = isset($map['properties'][$property]['property_group']) ? $map['properties'][$property]['property_group'] : reset($map['properties'][$property]['form_parents']);
      $form = _form_builder_webform_build_edit_form($component_type, $element, $property, $property_group, $map['properties'][$property]['form_parents'], $map['properties'][$property]['storage_parents']);
    }
  }
  return $form;
}

/**
 * Helper function; Save an element to a Webform component based on a map.
 */
function _form_builder_webform_save_mapped_component($component, $element) {
  if ($map = form_builder_webform_component_invoke($component['type'], 'form_builder_map')) {
    foreach ($map['properties'] as $property => $property_map) {
      if (isset($element['#' . $property]) && isset($property_map['storage_parents'])) {
        _form_builder_webform_save_mapped_component_value($component, $element['#' . $property], $property_map['storage_parents']);
      }
    }
  }
  return $component;
}

/**
 * Recursive function to set the value of a $component property.
 */
function _form_builder_webform_save_mapped_component_value(&$component, $value, $parents) {
  $parent = array_shift($parents);
  if (empty($parents)) {
    $component[$parent] = $value;
  }
  else {
    _form_builder_webform_save_mapped_component_value($component[$parent], $value, $parents);
  }
}

/**
 * Helper function; Alter a form element upon load based on a map.
 */
function _form_builder_webform_set_mapped_type($element) {
  if ($map = form_builder_webform_component_invoke($element['#webform_component']['type'], 'form_builder_map')) {
    // Keep the internal type of this element as the component type. For example
    // this may match an $element['#type'] of 'webform_date' and set the
    // $element['#form_builder']['element_type'] to simply 'date'.
    $element['#form_builder']['element_type'] = $map['form_builder_type'];
  }
  return $element;
}

/**
 * Helper function; Generate a default FAPI element for any component type.
 */
function _form_builder_webform_default($component_type, $merge_extras = array()) {
  $default_component = webform_component_invoke($component_type, 'defaults');
  $default_component['extra'] = isset($default_component['extra']) ? array_merge($default_component['extra'], $merge_extras) : $merge_extras;
  $default_element = webform_component_invoke($component_type, 'render', $default_component, NULL, FALSE);

  // Set a bare-minimum component for identification of the component type.
  $default_element['#webform_component']['type'] = $component_type;

  // Call the loading function to make sure that the default element gets the
  // same treatment as an existing one.
  $default_element = _form_builder_webform_set_mapped_type($default_element);

  return $default_element;
}

/**
 * Helper function; builds a form for editing part of a webform component.
 *
 * The returned form is derived from a subcomponent of the component form
 * provided by the Webform module.
 *
 * @param $component_type
 *   The webform component type to be edited.
 * @param $element
 *   A form array representing the element whose configuration form we are
 *   building.
 * @param $property
 *   The property of $element which stores the state of portions of the webform
 *   component that this form is responsible for configuring. The property
 *   should be passed in without the leading "#".
 * @param $form_builder_property_group
 *   The Form Builder property group in which this configuration form should be
 *   displayed.
 * @param $form_nested_keys
 *   An array of nested keys representing the location of the subcomponent of
 *   the _webform_edit_[component]() form that this configuration form will be
 *   taken from. For example, if the part of the configuration form we are
 *   interested in is located in $form['display']['width'], where $form is the
 *   output of _webform_edit_[component](), we would pass
 *   array('display', 'width') in for this parameter.
 * @param $component_nested_keys
 *   An array of nested keys representing the location of the portions of the
 *   webform component that this form is responsible for configuring. For
 *   example, if this form configures the data that is stored in
 *   $component['extra']['filtering'], where $component has the structure of
 *   the array returned by _webform_defaults_[component](), we would pass
 *   array('extra', 'filtering') in for this parameter.
 *
 * @return
 *   A form array that can be used to edit the specified part of the webform
 *   component represented by $element.
 */
function _form_builder_webform_build_edit_form($component_type, $element, $property, $form_builder_property_group, $form_nested_keys, $component_nested_keys) {
  // The Webform module stores existing component data as part of the passed-in
  // element. If the component doesn't exist yet, initialize a default
  // component.
  $defaults_function = '_webform_defaults_' . $component_type;
  $component = isset($element['#webform_component']) ? $element['#webform_component'] : $defaults_function();

  // The most up-to-date configuration data stored by Form Builder for the
  // part of the component we are editing is also stored in the passed-in
  // element, and should always take precedence.
  if (isset($element["#$property"])) {
    drupal_array_set_nested_value($component, $component_nested_keys, $element["#$property"]);
  }

  // Build the entire _webform_edit_file() form based on the current state of
  // the component, and obtain the slice of it that we want.
  $edit_function = '_webform_edit_' . $component_type;
  $form = $edit_function($component);
  $form = drupal_array_get_nested_value($form, $form_nested_keys);

  // Force the form to have a consistent #tree structure so it will appear in
  // $form_state['values'] the way we want.
  _form_builder_webform_force_tree($form);

  // Indicate the Form Builder property group that this form will be displayed
  // in.
  if ($form_builder_property_group) {
    $form['#form_builder']['property_group'] = $form_builder_property_group;
  }

  // Return the form, keyed by the name of the property that is being
  // configured.
  return array($property => $form);
}

/**
 * Helper function; replaces custom form #parents with a #tree structure.
 *
 * This is a helper function to force #tree = TRUE on all parts of a form,
 * regardless of any custom #parents that were originally defined as part of
 * the form. It is used to ensure a consistent structure within
 * $form_state['values'] when the form is submitted.
 *
 * @param $form
 *   The form array to modify.
 */
function _form_builder_webform_force_tree(&$form) {
  unset($form['#parents']); 
  $form['#tree'] = TRUE; 
  foreach (element_children($form) as $key) {
    _form_builder_webform_force_tree($form[$key]);
  }
}
