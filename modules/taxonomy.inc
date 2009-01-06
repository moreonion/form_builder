<?php
// $Id$

/**
 * @file
 *   Form builder implementation for taxonomy.module.
 */

/**
 * Implementation of hook_form_builder_types().
 */
function taxonomy_form_builder_types() {
  $fields = array();

  $vocabularies = taxonomy_get_vocabularies();

  $fields['taxonomy'] = array(
    'title' => t('Taxonomy'),
    'properties' => array(
      'taxonomy_vocabularies',
    ),
    'default' => array(
      '#key' => 'taxonomy',
      // Pseudo-type, this is replaced in taxonomy_form_builder_preview_alter().
      '#type' => 'taxonomy',
      '#taxonomy_vocabularies' => array(array_shift(array_keys($vocabularies))),
    ),
    'unique' => TRUE,
    'palette_group' => 'special',
  );

  return array(
    'node' => $fields,
    'example' => $fields,
  );
}

/**
 * Implementation of hook_form_builder_properties().
 */
function taxonomy_form_builder_properties() {
  return array(
    'taxonomy_vocabularies' => array(
      'form' => 'taxonomy_form_builder_configuration',
    ),
  );
}

/**
 * Form builder property form callback.
 */
function taxonomy_form_builder_configuration(&$form_state, $form_type, $element) {
  $form = array();

  $vocabularies = taxonomy_get_vocabularies();
  $options = array();
  foreach ($vocabularies as $vid => $vocabulary) {
    $options[$vid] = $vocabulary->name;
  }

  $form['taxonomy_vocabularies'] = array(
    '#title' => t('Enabled vocabularies'),
    '#type' => 'checkboxes',
    '#default_value' => $element['#taxonomy_vocabularies'],
    '#options' => $options,
    '#required' => TRUE,
    '#description' => t('Configure taxonomy element properties or add new vocabularies by visiting the <a href="!url">Taxonomy administration section</a>. If no vocabularies are desired for this form, remove the taxonomy element entirely.', array('!url' => url('admin/content/taxonomy'))),
  );

  return $form;
}

/**
 * Implementation of hook_form_builder_preview_alter().
 *
 * Convert the #type => taxonomy pseudo-element into a real fieldset.
 */
function taxonomy_form_builder_preview_alter(&$element, $form_type, $form_id) {
  if ($element['#type'] == 'taxonomy') {
    $vocabularies = array_intersect_key(taxonomy_get_vocabularies(), array_flip($element['#taxonomy_vocabularies']));
    $terms = array();
    $form = taxonomy_node_form($vocabularies, $terms);
    unset($element['#type']);
    $element = array_merge($element, $form['taxonomy']);
  }
}

/**
 * A straight copy/paste of taxonomy_form_alter(). Get the taxonomy node form.
 *
 * @todo Put this function directly in taxonomy.module, abstracting it from
 * taxonomy_form_alter().
 */
function taxonomy_node_form($vocabularies, $terms) {
  $form = array();

  if (!variable_get('taxonomy_override_selector', FALSE)) {
    foreach ($vocabularies as $vocabulary) {
      if ($vocabulary->tags) {
        if (isset($form_state['node_preview'])) {
          // Typed string can be changed by the user before preview,
          // so we just insert the tags directly as provided in the form.
          $typed_string = $terms['tags'][$vocabulary->vid];
        }
        else {
          $typed_string = taxonomy_implode_tags($terms, $vocabulary->vid) . (array_key_exists('tags', $terms) ? $terms['tags'][$vocabulary->vid] : NULL);
        }
        if ($vocabulary->help) {
          $help = $vocabulary->help;
        }
        else {
          $help = t('A comma-separated list of terms describing this content. Example: funny, bungee jumping, "Company, Inc.".');
        }
        $form['taxonomy']['tags'][$vocabulary->vid] = array('#type' => 'textfield',
          '#title' => $vocabulary->name,
          '#description' => $help,
          '#required' => $vocabulary->required,
          '#default_value' => $typed_string,
          '#autocomplete_path' => 'taxonomy/autocomplete/'. $vocabulary->vid,
          '#weight' => $vocabulary->weight,
          '#maxlength' => 255,
        );
      }
      else {
        // Extract terms belonging to the vocabulary in question.
        $default_terms = array();
        foreach ($terms as $term) {
          // Free tagging has no default terms and also no vid after preview.
          if (isset($term->vid) && $term->vid == $vocabulary->vid) {
            $default_terms[$term->tid] = $term;
          }
        }
        $form['taxonomy'][$vocabulary->vid] = taxonomy_form($vocabulary->vid, array_keys($default_terms), $vocabulary->help);
        $form['taxonomy'][$vocabulary->vid]['#weight'] = $vocabulary->weight;
        $form['taxonomy'][$vocabulary->vid]['#required'] = $vocabulary->required;
      }
    }
    if (!empty($form['taxonomy']) && is_array($form['taxonomy'])) {
      if (count($form['taxonomy']) > 1) {
        // Add fieldset only if form has more than 1 element.
        $form['taxonomy'] += array(
          '#type' => 'fieldset',
          '#title' => t('Vocabularies'),
          '#collapsible' => TRUE,
          '#collapsed' => FALSE,
        );
      }
      $form['taxonomy']['#weight'] = -3;
      $form['taxonomy']['#tree'] = TRUE;
    }
  }

  return $form;
}
